package org.wysaid.common;

import android.annotation.SuppressLint;
import android.opengl.EGL14;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyang on 15/12/13.
 */

@SuppressLint("InlinedApi")
public class SharedContext {
    public static final String LOG_TAG = Common.LOG_TAG;
    public static final int EGL_RECORDABLE_ANDROID = 0x3142;

    private EGLContext eglContext;
    private EGLConfig eglConfig;
    private EGLDisplay eglDisplay;
    private EGLSurface eglSurface;
    private EGL10 egl10;
    private GL10 gl10;

    private static int mBitsR = 8, mBitsG = 8, mBitsB = 8, mBitsA = 8;

    //注意， 设置之后将影响之后的所有操作
    static public void setContextColorBits(int r, int g, int b, int a) {
        mBitsR = r;
        mBitsG = g;
        mBitsB = b;
        mBitsA = a;
    }

    public static SharedContext create() {
        return create(EGL10.EGL_NO_CONTEXT, 64, 64, EGL10.EGL_PBUFFER_BIT, null);
    }

    public static SharedContext create(int width, int height) {
        return create(EGL10.EGL_NO_CONTEXT, width, height, EGL10.EGL_PBUFFER_BIT, null);
    }

    public static SharedContext create(EGLContext context, int width, int height) {
        return create(context, width, height, EGL10.EGL_PBUFFER_BIT, null);
    }

    //contextType: EGL10.EGL_PBUFFER_BIT
    //             EGL10.EGL_WINDOW_BIT
    //             EGL10.EGL_PIXMAP_BIT
    //             EGL_RECORDABLE_ANDROID ( = 0x3142 )
    //             etc.
    public static SharedContext create(EGLContext eglContext, int width, int height, int contextType, Object obj) {

        SharedContext sharedContext = new SharedContext();
        if (!sharedContext.initEGL(eglContext, width, height, contextType, obj)) {
            sharedContext.release();
            sharedContext = null;
        }
        return sharedContext;
    }

    public EGLContext getContext() {
        return eglContext;
    }

    public EGLDisplay getDisplay() {
        return eglDisplay;
    }

    public EGLSurface getSurface() {
        return eglSurface;
    }

    public EGL10 getEGL() {
        return egl10;
    }

    public GL10 getGL() {
        return gl10;
    }

    SharedContext() {
    }

    public void release() {
        Log.i(LOG_TAG, "#### CGESharedGLContext Destroying context... ####");
        if (eglDisplay != EGL10.EGL_NO_DISPLAY) {
            egl10.eglMakeCurrent(eglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
            egl10.eglDestroyContext(eglDisplay, eglContext);
            egl10.eglDestroySurface(eglDisplay, eglSurface);
            egl10.eglTerminate(eglDisplay);
        }

        eglDisplay = EGL10.EGL_NO_DISPLAY;
        eglSurface = EGL10.EGL_NO_SURFACE;
        eglContext = EGL10.EGL_NO_CONTEXT;
    }

    public void makeCurrent() {
        if (!egl10.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
            Log.e(LOG_TAG, "eglMakeCurrent failed:" + egl10.eglGetError());
        }
    }

    public boolean swapBuffers() {
        return egl10.eglSwapBuffers(eglDisplay, eglSurface);
    }

    private boolean initEGL(EGLContext context, int width, int height, int contextType, Object obj) {

        int[] contextAttribList = {
                EGL14.EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL10.EGL_NONE
        };

        int[] configSpec = {
                EGL10.EGL_SURFACE_TYPE, contextType,
                EGL10.EGL_RENDERABLE_TYPE, EGL14.EGL_OPENGL_ES2_BIT,
                EGL10.EGL_RED_SIZE, 8, EGL10.EGL_GREEN_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 8, EGL10.EGL_ALPHA_SIZE, 8,
                EGL10.EGL_NONE
        };

        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfig = new int[1];
        int[] version = new int[2];

        int surfaceAttribList[] = {
                EGL10.EGL_WIDTH, width,
                EGL10.EGL_HEIGHT, height,
                EGL10.EGL_NONE
        };

        egl10 = (EGL10) EGLContext.getEGL();

        if ((eglDisplay = egl10.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY)) == EGL10.EGL_NO_DISPLAY) {
            Log.e(LOG_TAG, String.format("eglGetDisplay() returned error 0x%x", egl10.eglGetError()));
            return false;
        }

        if (!egl10.eglInitialize(eglDisplay, version)) {
            Log.e(LOG_TAG, String.format("eglInitialize() returned error 0x%x", egl10.eglGetError()));
            return false;
        }

        Log.i(LOG_TAG, String.format("eglInitialize - major: %d, minor: %d", version[0], version[1]));

        if (!egl10.eglChooseConfig(eglDisplay, configSpec, configs, 1, numConfig)) {
            Log.e(LOG_TAG, String.format("eglChooseConfig() returned error 0x%x", egl10.eglGetError()));
            return false;
        }

        Log.i(LOG_TAG, String.format("Config num: %d, has sharedContext: %s", numConfig[0], context == EGL10.EGL_NO_CONTEXT ? "NO" : "YES"));

        eglConfig = configs[0];

        eglContext = egl10.eglCreateContext(eglDisplay, eglConfig,
                context, contextAttribList);
        if (eglContext == EGL10.EGL_NO_CONTEXT) {
            Log.e(LOG_TAG, "eglCreateContext Failed!");
            return false;
        }

        switch (contextType) {
            case EGL10.EGL_PIXMAP_BIT:
                eglSurface = egl10.eglCreatePixmapSurface(eglDisplay, eglConfig, obj, surfaceAttribList);
                break;
            case EGL10.EGL_WINDOW_BIT:
                eglSurface = egl10.eglCreateWindowSurface(eglDisplay, eglConfig, obj, surfaceAttribList);
                break;
            case EGL10.EGL_PBUFFER_BIT:
            case EGL_RECORDABLE_ANDROID:
                eglSurface = egl10.eglCreatePbufferSurface(eglDisplay, eglConfig,
                        surfaceAttribList);
        }

        if (eglSurface == EGL10.EGL_NO_SURFACE) {
            Log.e(LOG_TAG, "eglCreatePbufferSurface Failed!");
            return false;
        }

        if (!egl10.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
            Log.e(LOG_TAG, "eglMakeCurrent failed:" + egl10.eglGetError());
            return false;
        }

        int[] clientVersion = new int[1];
        egl10.eglQueryContext(eglDisplay, eglContext, EGL14.EGL_CONTEXT_CLIENT_VERSION, clientVersion);
        Log.i(LOG_TAG, "EGLContext created, client version " + clientVersion[0]);

        gl10 = (GL10) eglContext.getGL();

        return true;
    }
}
