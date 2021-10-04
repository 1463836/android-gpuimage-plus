package org.wysaid.view;

import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import org.wysaid.camera.CameraInstance;
import org.wysaid.common.Common;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/7/17.
 */

public class CameraGLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer {

    public CameraGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 0, 0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
//        setZOrderOnTop(true);
//        setZOrderMediaOverlay(true);
    }

    public static final String LOG_TAG = Common.LOG_TAG;

    public int mMaxTextureSize = 0;

    protected int viewWidth;
    protected int viewHeight;

    protected int recordWidth = 480;
    protected int recordHeight = 640;

    //isBigger 为true 表示当宽高不满足时，取最近的较大值.
    // 若为 false 则取较小的
    public void setPictureSize(int width, int height, boolean isBigger) {
        //默认会旋转90度.
        CameraInstance.getInstance().setPictureSize(height, width, isBigger);
    }

    // mode value should be:
    //    Camera.Parameters.FLASH_MODE_AUTO;
    //    Camera.Parameters.FLASH_MODE_OFF;
    //    Camera.Parameters.FLASH_MODE_ON;
    //    Camera.Parameters.FLASH_MODE_RED_EYE
    //    Camera.Parameters.FLASH_MODE_TORCH 等
    public synchronized boolean setFlashLightMode(String mode) {

        if (!getContext().getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FLASH)) {
            Log.e(LOG_TAG, "No flash light is supported by current device!");
            return false;
        }

        if (!isCameraBackForward) {
            return false;
        }

        Camera.Parameters parameters = CameraInstance.getInstance().getParams();

        if (parameters == null)
            return false;

        try {

            if (!parameters.getSupportedFlashModes().contains(mode)) {
                Log.e(LOG_TAG, "Invalid Flash Light Mode!!!");
                return false;
            }

            parameters.setFlashMode(mode);
            CameraInstance.getInstance().setParams(parameters);
        } catch (Exception e) {
            Log.e(LOG_TAG, "Switch flash light failed, check if you're using front camera.");
            return false;
        }

        return true;
    }

    protected int maxPreviewWidth = 1280;
    protected int maxPreviewHeight = 1280;

    public static class Viewport {
        public int x, y, width, height;
    }

    protected Viewport drawViewport = new Viewport();

    public Viewport getDrawViewport() {
        return drawViewport;
    }

    //The max preview size. Change it to 1920+ if you want to preview with 1080P
    void setMaxPreviewSize(int w, int h) {
        maxPreviewWidth = w;
        maxPreviewHeight = h;
    }

    protected boolean fitFullView = false;

    public void setFitFullView(boolean fit) {
        fitFullView = fit;
        calcViewport();
    }

    //是否使用后置摄像头
    protected boolean isCameraBackForward = true;

    public boolean isCameraBackForward() {
        return isCameraBackForward;
    }


    //should be called before 'onSurfaceCreated'.
    public void presetCameraForward(boolean isBackForward) {
        isCameraBackForward = isBackForward;
    }

    //注意， 录制的尺寸将影响preview的尺寸
    //这里的width和height表示竖屏尺寸
    //在onSurfaceCreated之前设置有效
    public void presetRecordingSize(int width, int height) {
        if (width > maxPreviewWidth || height > maxPreviewHeight) {
            float scaling = Math.min(maxPreviewWidth / (float) width, maxPreviewHeight / (float) height);
            width = (int) (width * scaling);
            height = (int) (height * scaling);
        }

        recordWidth = width;
        recordHeight = height;
        CameraInstance.getInstance().setPreferPreviewSize(width, height);
    }

    public void resumePreview() {

    }

    public void stopPreview() {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                CameraInstance.getInstance().stopPreview();
            }
        });
    }

    protected void onSwitchCamera() {

    }

    public final void switchCamera() {
        isCameraBackForward = !isCameraBackForward;

        queueEvent(new Runnable() {
            @Override
            public void run() {

                CameraInstance.getInstance().stopCamera();
                onSwitchCamera();
                int facing = isCameraBackForward ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;

                CameraInstance.getInstance().tryOpenCamera(new CameraInstance.CameraOpenCallback() {
                    @Override
                    public void cameraReady() {
                        resumePreview();
                    }
                }, facing);

                requestRender();
            }
        });
    }

    //Attention， 'focusAtPoint' will change focus mode to 'FOCUS_MODE_AUTO'
    //If you want to keep the previous focus mode， please reset the focus mode after 'AutoFocusCallback'.
    //x,y should be: [0, 1]， stands for 'touchEventPosition / viewSize'.
    public void focusAtPoint(float x, float y, Camera.AutoFocusCallback focusCallback) {
        CameraInstance.getInstance().focusAtPoint(y, 1.0f - x, focusCallback);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
        CameraInstance.getInstance().stopCamera();
    }

    public interface OnCreateCallback {
        void createOver();
    }

    protected OnCreateCallback mOnCreateCallback;

    //定制一些初始化操作
    public void setOnCreateCallback(final OnCreateCallback callback) {
        mOnCreateCallback = callback;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(LOG_TAG, "onSurfaceCreated...");

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_STENCIL_TEST);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);

        int texSize[] = new int[1];

        GLES20.glGetIntegerv(GLES20.GL_MAX_TEXTURE_SIZE, texSize, 0);
        mMaxTextureSize = texSize[0];

        if (mOnCreateCallback != null) {
            mOnCreateCallback.createOver();
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i(LOG_TAG, String.format("onSurfaceChanged: %d x %d", width, height));

        GLES20.glClearColor(0, 0, 0, 0);

        viewWidth = width;
        viewHeight = height;

        calcViewport();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        //Nothing . See `CameraGLSurfaceViewWithTexture` or `CameraGLSurfaceViewWithBuffer`
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.i(LOG_TAG, "glsurfaceview onResume...");
    }

    @Override
    public void onPause() {
        Log.i(LOG_TAG, "glsurfaceview onPause in...");

        CameraInstance.getInstance().stopCamera();
        super.onPause();
        Log.i(LOG_TAG, "glsurfaceview onPause out...");
    }

    public interface ReleaseOKCallback {

        void releaseOK();
    }

    protected void onRelease() {

    }

    public final void release(final ReleaseOKCallback callback) {
        queueEvent(new Runnable() {
            @Override
            public void run() {

                onRelease();

                Log.i(LOG_TAG, "GLSurfaceview release...");
                if (callback != null)
                    callback.releaseOK();
            }
        });
    }

    public interface TakePictureCallback {
        //You can recycle the bitmap.
        void takePictureOK(Bitmap bmp);
    }

    public void takeShot(final TakePictureCallback callback) {
    }

    protected void calcViewport() {

        float scaling = recordWidth / (float) recordHeight;
        float viewRatio = viewWidth / (float) viewHeight;
        float s = scaling / viewRatio;

        int w, h;

        if (fitFullView) {
            //撑满全部view(内容大于view)
            if (s > 1.0) {
                w = (int) (viewHeight * scaling);
                h = viewHeight;
            } else {
                w = viewWidth;
                h = (int) (viewWidth / scaling);
            }
        } else {
            //显示全部内容(内容小于view)
            if (s > 1.0) {
                w = viewWidth;
                h = (int) (viewWidth / scaling);
            } else {
                h = viewHeight;
                w = (int) (viewHeight * scaling);
            }
        }

        drawViewport.width = w;
        drawViewport.height = h;
        drawViewport.x = (viewWidth - drawViewport.width) / 2;
        drawViewport.y = (viewHeight - drawViewport.height) / 2;
        Log.w(LOG_TAG, String.format("calcViewport : x %d, y %d, drawViewport w %d, drawViewport h %d,viewWidth w %d, viewHeight h %d",
                drawViewport.x, drawViewport.y, drawViewport.width, drawViewport.height,viewWidth,viewHeight));
    }
}
