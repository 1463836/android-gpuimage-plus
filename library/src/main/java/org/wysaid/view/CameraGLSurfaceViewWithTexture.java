package org.wysaid.view;

/**
 * Created by wangyang on 15/7/27.
 */


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.media.ExifInterface;
import android.opengl.GLES20;
import android.util.AttributeSet;
import android.util.Log;

import org.wysaid.camera.CameraInstance;
import org.wysaid.common.Common;
import org.wysaid.common.FrameBufferObject;
import org.wysaid.nativePort.CGENativeLibrary;
import org.wysaid.nativePort.FrameRecorder;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/7/17.
 */
public class CameraGLSurfaceViewWithTexture extends CameraGLSurfaceView implements SurfaceTexture.OnFrameAvailableListener {

    protected SurfaceTexture surfaceTexture;
    protected int textureOES;
    protected boolean mIsTransformMatrixSet = false;
    protected FrameRecorder frameRecorder;

    public FrameRecorder getRecorder() {
        return frameRecorder;
    }


    public synchronized void setFilterWithConfig(final String config) {
        queueEvent(new Runnable() {
            @Override
            public void run() {

                if (frameRecorder != null) {
                    frameRecorder.setFilterWidthConfig(config);
                } else {
                    Log.e(LOG_TAG, "setFilterWithConfig after release!!");
                }
            }
        });
    }

    public void setFilterIntensity(final float intensity) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                if (frameRecorder != null) {
                    frameRecorder.setFilterIntensity(intensity);
                } else {
                    Log.e(LOG_TAG, "setFilterIntensity after release!!");
                }
            }
        });
    }

    //定制一些初始化操作
    public void setOnCreateCallback(final OnCreateCallback callback) {
        if (frameRecorder == null || callback == null) {
            onCreateCallback = callback;
        } else {
            // Already created, just run.
            queueEvent(new Runnable() {
                @Override
                public void run() {
                    callback.createOver();
                }
            });
        }
    }

    public CameraGLSurfaceViewWithTexture(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        frameRecorder = new FrameRecorder();
        mIsTransformMatrixSet = false;
        if (!frameRecorder.init(widthVideo, heightVideo, widthVideo, heightVideo)) {
            Log.e(LOG_TAG, "Frame Recorder init failed!");
        }

        frameRecorder.setSrcRotation((float) (Math.PI / 2.0));
        frameRecorder.setSrcFlipScale(1.0f, -1.0f);
        frameRecorder.setRenderFlipScale(1.0f, -1.0f);

        textureOES = Common.genSurfaceTextureID();
        surfaceTexture = new SurfaceTexture(textureOES);
        surfaceTexture.setOnFrameAvailableListener(this);

        super.onSurfaceCreated(gl, config);
    }


    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        super.onSurfaceChanged(gl, width, height);

        if (!CameraInstance.getInstance().isPreviewing()) {
            resumePreview();
        }
    }

    public void resumePreview() {

        if (frameRecorder == null) {
            Log.e(LOG_TAG, "resumePreview after release!!");
            return;
        }

        if (!CameraInstance.getInstance().isCameraOpened()) {

            int facing = isCameraBackForward ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;

            CameraInstance.getInstance().tryOpenCamera(new CameraInstance.CameraOpenCallback() {
                @Override
                public void cameraReady() {
//                    Log.i(LOG_TAG, "tryOpenCamera OK...");
                }
            }, facing);
        }

        if (!CameraInstance.getInstance().isPreviewing()) {
            CameraInstance.getInstance().startPreview(surfaceTexture);
            frameRecorder.srcResize(CameraInstance.getInstance().previewHeight(), CameraInstance.getInstance().previewWidth());
        }

        requestRender();
    }

    protected float[] mTransformMatrix = new float[16];

    @Override
    public void onDrawFrame(GL10 gl) {

        if (surfaceTexture == null || !CameraInstance.getInstance().isPreviewing()) {
            return;
        }

        surfaceTexture.updateTexImage();

        surfaceTexture.getTransformMatrix(mTransformMatrix);
        frameRecorder.update(textureOES, mTransformMatrix);

        frameRecorder.runProc();

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glClearColor(0, 0, 0, 0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        frameRecorder.render(drawViewport.x, drawViewport.y, drawViewport.width, drawViewport.height);
    }

//    protected long mTimeCount2 = 0;
//    protected long mFramesCount2 = 0;
//    protected long mLastTimestamp2 = 0;

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

        requestRender();

//        if (mLastTimestamp2 == 0)
//            mLastTimestamp2 = System.currentTimeMillis();
//
//        long currentTimestamp = System.currentTimeMillis();
//
//        ++mFramesCount2;
//        mTimeCount2 += currentTimestamp - mLastTimestamp2;
//        mLastTimestamp2 = currentTimestamp;
//        if (mTimeCount2 >= 1000) {
//            Log.i(LOG_TAG, String.format("camera sample rate: %d", mFramesCount2));
//            mTimeCount2 %= 1000;
//            mFramesCount2 = 0;
//        }
    }

    protected void onRelease() {
        super.onRelease();
        if (surfaceTexture != null) {
            surfaceTexture.release();
            surfaceTexture = null;
        }

        if (textureOES != 0) {
            Common.deleteTextureID(textureOES);
            textureOES = 0;
        }

        if (frameRecorder != null) {
            frameRecorder.release();
            frameRecorder = null;
        }
    }

    @Override
    protected void onSwitchCamera() {
        super.onSwitchCamera();
        if (frameRecorder != null) {
            frameRecorder.setSrcRotation((float) (Math.PI / 2.0));
            frameRecorder.setRenderFlipScale(1.0f, -1.0f);
        }
    }

    @Override
    public void takeShot(final TakePictureCallback callback) {
        assert callback != null : "callback must not be null!";

        if (frameRecorder == null) {
            Log.e(LOG_TAG, "Recorder not initialized!");
            callback.takePictureOK(null);
            return;
        }

        queueEvent(new Runnable() {
            @Override
            public void run() {

                FrameBufferObject frameBufferObject = new FrameBufferObject();
                int bufferTexID;
                IntBuffer buffer;
                Bitmap bmp;

                bufferTexID = Common.genBlankTextureID(widthVideo, heightVideo);
                frameBufferObject.bindTexture(bufferTexID);
                GLES20.glViewport(0, 0, widthVideo, heightVideo);
                frameRecorder.drawCache();
                buffer = IntBuffer.allocate(widthVideo * heightVideo);
                GLES20.glReadPixels(0, 0, widthVideo, heightVideo, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buffer);
                bmp = Bitmap.createBitmap(widthVideo, heightVideo, Bitmap.Config.ARGB_8888);
                bmp.copyPixelsFromBuffer(buffer);
                Log.i(LOG_TAG, String.format("w: %d, h: %d", widthVideo, heightVideo));

                frameBufferObject.release();
                GLES20.glDeleteTextures(1, new int[]{bufferTexID}, 0);

                callback.takePictureOK(bmp);
            }
        });

    }

    //isBigger 为true 表示当宽高不满足时，取最近的较大值.
    // 若为 false 则取较小的
    public void setPictureSize(int width, int height, boolean isBigger) {
        //默认会旋转90度.
        CameraInstance.getInstance().setPictureSize(height, width, isBigger);
    }

    public synchronized void takePicture(final TakePictureCallback photoCallback, Camera.ShutterCallback shutterCallback, final String config, final float intensity, final boolean isFrontMirror) {

        Camera.Parameters params = CameraInstance.getInstance().getParams();

        if (photoCallback == null || params == null) {
            Log.e(LOG_TAG, "takePicture after release!");
            if (photoCallback != null) {
                photoCallback.takePictureOK(null);
            }
            return;
        }

        try {
            params.setRotation(90);
            CameraInstance.getInstance().setParams(params);
        } catch (Exception e) {
            Log.e(LOG_TAG, "Error when takePicture: " + e.toString());
            if (photoCallback != null) {
                photoCallback.takePictureOK(null);
            }
            return;
        }

        CameraInstance.getInstance().getCameraDevice().takePicture(shutterCallback, null, new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(final byte[] data, Camera camera) {

                Camera.Parameters params = camera.getParameters();
                Camera.Size sz = params.getPictureSize();

                boolean shouldRotate;

                Bitmap bmp;
                int width, height;

                //当拍出相片不为正方形时， 可以判断图片是否旋转
                if (sz.width != sz.height) {
                    //默认数据格式已经设置为 JPEG
                    bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
                    width = bmp.getWidth();
                    height = bmp.getHeight();
                    shouldRotate = (sz.width > sz.height && width > height) || (sz.width < sz.height && width < height);
                } else {
                    Log.i(LOG_TAG, "Cache image to get exif.");

                    try {
                        String tmpFilename = getContext().getExternalCacheDir() + "/picture_cache000.jpg";
                        FileOutputStream fileout = new FileOutputStream(tmpFilename);
                        BufferedOutputStream bufferOutStream = new BufferedOutputStream(fileout);
                        bufferOutStream.write(data);
                        bufferOutStream.flush();
                        bufferOutStream.close();

                        ExifInterface exifInterface = new ExifInterface(tmpFilename);
                        int orientation = exifInterface.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);

                        switch (orientation) {
                            //被保存图片exif记录只有旋转90度， 和不旋转两种情况
                            case ExifInterface.ORIENTATION_ROTATE_90:
                                shouldRotate = true;
                                break;
                            default:
                                shouldRotate = false;
                                break;
                        }

                        bmp = BitmapFactory.decodeFile(tmpFilename);
                        width = bmp.getWidth();
                        height = bmp.getHeight();

                    } catch (IOException e) {
                        Log.e(LOG_TAG, "Err when saving bitmap...");
                        e.printStackTrace();
                        return;
                    }
                }


                if (width > mMaxTextureSize || height > mMaxTextureSize) {
                    float scaling = Math.max(width / (float) mMaxTextureSize, height / (float) mMaxTextureSize);
                    Log.i(LOG_TAG, String.format("目标尺寸(%d x %d)超过当前设备OpenGL 能够处理的最大范围(%d x %d)， 现在将图片压缩至合理大小!", width, height, mMaxTextureSize, mMaxTextureSize));

                    bmp = Bitmap.createScaledBitmap(bmp, (int) (width / scaling), (int) (height / scaling), false);

                    width = bmp.getWidth();
                    height = bmp.getHeight();
                }

                Bitmap bmp2;

                if (shouldRotate) {
                    bmp2 = Bitmap.createBitmap(height, width, Bitmap.Config.ARGB_8888);

                    Canvas canvas = new Canvas(bmp2);

                    if (CameraInstance.getInstance().getFacing() == Camera.CameraInfo.CAMERA_FACING_BACK) {
                        Matrix mat = new Matrix();
                        int halfLen = Math.min(width, height) / 2;
                        mat.setRotate(90, halfLen, halfLen);
                        canvas.drawBitmap(bmp, mat, null);
                    } else {
                        Matrix mat = new Matrix();

                        if (isFrontMirror) {
                            mat.postTranslate(-width / 2, -height / 2);
                            mat.postScale(-1.0f, 1.0f);
                            mat.postTranslate(width / 2, height / 2);
                            int halfLen = Math.min(width, height) / 2;
                            mat.postRotate(90, halfLen, halfLen);
                        } else {
                            int halfLen = Math.max(width, height) / 2;
                            mat.postRotate(-90, halfLen, halfLen);
                        }

                        canvas.drawBitmap(bmp, mat, null);
                    }

                    bmp.recycle();
                } else {
                    if (CameraInstance.getInstance().getFacing() == Camera.CameraInfo.CAMERA_FACING_BACK) {
                        bmp2 = bmp;
                    } else {

                        bmp2 = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                        Canvas canvas = new Canvas(bmp2);
                        Matrix mat = new Matrix();
                        if (isFrontMirror) {
                            mat.postTranslate(-width / 2, -height / 2);
                            mat.postScale(1.0f, -1.0f);
                            mat.postTranslate(width / 2, height / 2);
                        } else {
                            mat.postTranslate(-width / 2, -height / 2);
                            mat.postScale(-1.0f, -1.0f);
                            mat.postTranslate(width / 2, height / 2);
                        }

                        canvas.drawBitmap(bmp, mat, null);
                    }

                }

                if (config != null) {
                    CGENativeLibrary.filterImage_MultipleEffectsWriteBack(bmp2, config, intensity);
                }

                photoCallback.takePictureOK(bmp2);

                CameraInstance.getInstance().getCameraDevice().startPreview();
            }
        });
    }
}
