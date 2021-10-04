package org.wysaid.camera;

import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Build;
import android.util.Log;

import org.wysaid.common.Common;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * Created by wangyang on 15/7/27.
 */


// Camera 仅适用单例
public class CameraInstance {
    public static final String LOG_TAG = Common.LOG_TAG;

    private static final String ASSERT_MSG = "检测到CameraDevice 为 null! 请检查";

    private Camera camera;
    private Camera.Parameters parameters;

    public static final int DEFAULT_PREVIEW_RATE = 30;


    private boolean mIsPreviewing = false;

    private int mDefaultCameraID = -1;

    private static CameraInstance mThisInstance;
    private int previewWidth;
    private int previewHeight;

    private int pictureWidth = 1000;
    private int pictureHeight = 1000;

    private int preferPreviewWidth = 640;
    private int preferPreviewHeight = 640;

    private int mFacing = 0;

    private CameraInstance() {
    }

    public static synchronized CameraInstance getInstance() {
        if (mThisInstance == null) {
            mThisInstance = new CameraInstance();
        }
        return mThisInstance;
    }

    public boolean isPreviewing() {
        return mIsPreviewing;
    }

    public int previewWidth() {
        return previewWidth;
    }

    public int previewHeight() {
        return previewHeight;
    }

    public int pictureWidth() {
        return pictureWidth;
    }

    public int pictureHeight() {
        return pictureHeight;
    }

    public void setPreferPreviewSize(int w, int h) {
        preferPreviewHeight = w;
        preferPreviewWidth = h;
    }

    public interface CameraOpenCallback {
        void cameraReady();
    }

    public boolean tryOpenCamera(CameraOpenCallback callback) {
        return tryOpenCamera(callback, Camera.CameraInfo.CAMERA_FACING_BACK);
    }

    public int getFacing() {
        return mFacing;
    }

    public synchronized boolean tryOpenCamera(CameraOpenCallback callback, int facing) {
        Log.i(LOG_TAG, "try open camera...");

        try {
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.FROYO) {
                int numberOfCameras = Camera.getNumberOfCameras();

                Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
                for (int i = 0; i < numberOfCameras; i++) {
                    Camera.getCameraInfo(i, cameraInfo);
                    if (cameraInfo.facing == facing) {
                        mDefaultCameraID = i;
                        mFacing = facing;
                        break;
                    }
                }
            }
            stopPreview();
            if (camera != null)
                camera.release();

            if (mDefaultCameraID >= 0) {
                camera = Camera.open(mDefaultCameraID);
            } else {
                camera = Camera.open();
                mFacing = Camera.CameraInfo.CAMERA_FACING_BACK; //default: back facing
            }
        } catch (Exception e) {
            Log.e(LOG_TAG, "Open Camera Failed!");
            e.printStackTrace();
            camera = null;
            return false;
        }

        if (camera != null) {
            Log.i(LOG_TAG, "Camera opened!");

            try {
                initCamera(DEFAULT_PREVIEW_RATE);
            } catch (Exception e) {
                camera.release();
                camera = null;
                return false;
            }

            if (callback != null) {
                callback.cameraReady();
            }

            return true;
        }

        return false;
    }

    public synchronized void stopCamera() {
        if (camera != null) {
            mIsPreviewing = false;
            camera.stopPreview();
            camera.setPreviewCallback(null);
            camera.release();
            camera = null;
        }
    }

    public boolean isCameraOpened() {
        return camera != null;
    }

    public synchronized void startPreview(SurfaceTexture texture, Camera.PreviewCallback callback) {
        Log.i(LOG_TAG, "Camera startPreview...");
        if (mIsPreviewing) {
            Log.e(LOG_TAG, "Err: camera is previewing...");
            return;
        }

        if (camera != null) {
            try {
                camera.setPreviewTexture(texture);
//                mCameraDevice.addCallbackBuffer(callbackBuffer);
//                mCameraDevice.setPreviewCallbackWithBuffer(callback);
                camera.setPreviewCallbackWithBuffer(callback);
            } catch (IOException e) {
                e.printStackTrace();
            }

            camera.startPreview();
            mIsPreviewing = true;
        }
    }

    public void startPreview(SurfaceTexture texture) {
        startPreview(texture, null);
    }

    public void startPreview(Camera.PreviewCallback callback) {
        startPreview(null, callback);
    }

    public synchronized void stopPreview() {
        if (mIsPreviewing && camera != null) {
            Log.i(LOG_TAG, "Camera stopPreview...");
            mIsPreviewing = false;
            camera.stopPreview();
        }
    }

    public synchronized Camera.Parameters getParams() {
        if (camera != null)
            return camera.getParameters();
        assert camera != null : ASSERT_MSG;
        return null;
    }

    public synchronized void setParams(Camera.Parameters param) {
        if (camera != null) {
            parameters = param;
            camera.setParameters(parameters);
        }
        assert camera != null : ASSERT_MSG;
    }

    public Camera getCameraDevice() {
        return camera;
    }

    //保证从大到小排列
    private Comparator<Camera.Size> comparatorBigger = new Comparator<Camera.Size>() {
        @Override
        public int compare(Camera.Size lhs, Camera.Size rhs) {
            int w = rhs.width - lhs.width;
            if (w == 0)
                return rhs.height - lhs.height;
            return w;
        }
    };

    //保证从小到大排列
    private Comparator<Camera.Size> comparatorSmaller = new Comparator<Camera.Size>() {
        @Override
        public int compare(Camera.Size lhs, Camera.Size rhs) {
            int w = lhs.width - rhs.width;
            if (w == 0)
                return lhs.height - rhs.height;
            return w;
        }
    };

    public void initCamera(int previewRate) {
        if (camera == null) {
            Log.e(LOG_TAG, "initCamera: Camera is not opened!");
            return;
        }

        parameters = camera.getParameters();
        List<Integer> supportedPictureFormats = parameters.getSupportedPictureFormats();

        for (int fmt : supportedPictureFormats) {
            Log.i(LOG_TAG, String.format("Picture Format: %x", fmt));
        }

        parameters.setPictureFormat(PixelFormat.JPEG);

        List<Camera.Size> picSizes = parameters.getSupportedPictureSizes();
        Camera.Size picSz = null;

        Collections.sort(picSizes, comparatorBigger);

        for (Camera.Size sz : picSizes) {
//            Log.i(LOG_TAG, String.format("Supported picture size: %d x %d", sz.width, sz.height));
            if (picSz == null || (sz.width >= pictureWidth && sz.height >= pictureHeight)) {
                picSz = sz;
            }
        }

        List<Camera.Size> prevSizes = parameters.getSupportedPreviewSizes();
        Camera.Size prevSz = null;

        Collections.sort(prevSizes, comparatorBigger);

        for (Camera.Size sz : prevSizes) {
//            Log.i(LOG_TAG, String.format("Supported preview size: %d x %d", sz.width, sz.height));
            if (prevSz == null || (sz.width >= preferPreviewWidth && sz.height >= preferPreviewHeight)) {
                prevSz = sz;
            }
        }

        List<Integer> frameRates = parameters.getSupportedPreviewFrameRates();

        int fpsMax = 0;

        for (Integer n : frameRates) {
//            Log.i(LOG_TAG, "Supported frame rate: " + n);
            if (fpsMax < n) {
                fpsMax = n;
            }
        }

        parameters.setPreviewSize(prevSz.width, prevSz.height);
        parameters.setPictureSize(picSz.width, picSz.height);

        List<String> focusModes = parameters.getSupportedFocusModes();
        if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        }

        previewRate = fpsMax;
        parameters.setPreviewFrameRate(previewRate); //设置相机预览帧率
//        mParams.setPreviewFpsRange(20, 60);

        try {
            camera.setParameters(parameters);
        } catch (Exception e) {
            e.printStackTrace();
        }


        parameters = camera.getParameters();

        Camera.Size szPic = parameters.getPictureSize();
        Camera.Size szPrev = parameters.getPreviewSize();

        previewWidth = szPrev.width;
        previewHeight = szPrev.height;

        pictureWidth = szPic.width;
        pictureHeight = szPic.height;

        Log.i(LOG_TAG, String.format("Camera Picture Size: %d x %d", szPic.width, szPic.height));
        Log.i(LOG_TAG, String.format("Camera Preview Size: %d x %d", szPrev.width, szPrev.height));
    }

    public synchronized void setFocusMode(String focusMode) {

        if (camera == null)
            return;

        parameters = camera.getParameters();
        List<String> focusModes = parameters.getSupportedFocusModes();
        if (focusModes.contains(focusMode)) {
            parameters.setFocusMode(focusMode);
        }
    }

    public synchronized void setPictureSize(int width, int height, boolean isBigger) {

        if (camera == null) {
            pictureWidth = width;
            pictureHeight = height;
            return;
        }

        parameters = camera.getParameters();


        List<Camera.Size> picSizes = parameters.getSupportedPictureSizes();
        Camera.Size size = null;

        if (isBigger) {
            Collections.sort(picSizes, comparatorBigger);
            for (Camera.Size sz : picSizes) {
                if (size == null || (sz.width >= width && sz.height >= height)) {
                    size = sz;
                }
            }
        } else {
            Collections.sort(picSizes, comparatorSmaller);
            for (Camera.Size sz : picSizes) {
                if (size == null || (sz.width <= width && sz.height <= height)) {
                    size = sz;
                }
            }
        }

        pictureWidth = size.width;
        pictureHeight = size.height;

        try {
            parameters.setPictureSize(pictureWidth, pictureHeight);
            camera.setParameters(parameters);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void focusAtPoint(float x, float y, final Camera.AutoFocusCallback callback) {
        focusAtPoint(x, y, 0.2f, callback);
    }

    public synchronized void focusAtPoint(float x, float y, float radius, final Camera.AutoFocusCallback callback) {
        if (camera == null) {
            Log.e(LOG_TAG, "Error: focus after release.");
            return;
        }

        parameters = camera.getParameters();

        if (parameters.getMaxNumMeteringAreas() > 0) {

            int focusRadius = (int) (radius * 1000.0f);
            int left = (int) (x * 2000.0f - 1000.0f) - focusRadius;
            int top = (int) (y * 2000.0f - 1000.0f) - focusRadius;

            Rect focusArea = new Rect();
            focusArea.left = Math.max(left, -1000);
            focusArea.top = Math.max(top, -1000);
            focusArea.right = Math.min(left + focusRadius, 1000);
            focusArea.bottom = Math.min(top + focusRadius, 1000);
            List<Camera.Area> meteringAreas = new ArrayList<Camera.Area>();
            meteringAreas.add(new Camera.Area(focusArea, 800));

            try {
                camera.cancelAutoFocus();
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
                parameters.setFocusAreas(meteringAreas);
                camera.setParameters(parameters);
                camera.autoFocus(callback);
            } catch (Exception e) {
                Log.e(LOG_TAG, "Error: focusAtPoint failed: " + e.toString());
            }
        } else {
            Log.i(LOG_TAG, "The device does not support metering areas...");
            try {
                camera.autoFocus(callback);
            } catch (Exception e) {
                Log.e(LOG_TAG, "Error: focusAtPoint failed: " + e.toString());
            }
        }

    }
}
