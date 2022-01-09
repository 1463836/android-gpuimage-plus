/*
* cgeSharedGLContext.cpp
*
*  Created on: 2015-7-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "SharedGLContext.h"
#include "CommonDefine.h"

EGLint CGESharedGLContext::s_bitR = 8;
EGLint CGESharedGLContext::s_bitG = 8;
EGLint CGESharedGLContext::s_bitB = 8;
EGLint CGESharedGLContext::s_bitA = 8;

void CGESharedGLContext::setContextColorBits(int r, int g, int b, int a) {
    s_bitR = r;
    s_bitG = g;
    s_bitB = b;
    s_bitA = a;
}

CGESharedGLContext *CGESharedGLContext::create() {
    CGESharedGLContext *context = new CGESharedGLContext();
    if (!context->init(EGL_NO_CONTEXT, 64, 64, PBUFFER)) {
        delete context;
        context = nullptr;
    }
    return context;
}

CGESharedGLContext *CGESharedGLContext::create(int width, int height) {
    CGESharedGLContext *context = new CGESharedGLContext();
    if (!context->init(EGL_NO_CONTEXT, width, height, PBUFFER)) {
        delete context;
        context = nullptr;
    }
    return context;
}

CGESharedGLContext *
CGESharedGLContext::create(EGLContext sharedContext, int width, int height, ContextType type) {
    CGESharedGLContext *context = new CGESharedGLContext();
    if (!context->init(sharedContext, width, height, type)) {
        delete context;
        context = nullptr;
    }
    return context;
}

CGESharedGLContext::~CGESharedGLContext() {
    destroy();
}

bool CGESharedGLContext::init(EGLContext eglContext, int width, int height, ContextType type) {

    if ((eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        loge("eglGetDisplay() returned error 0x%x", eglGetError());
        return false;
    }

    EGLint major, minor;

    if (!eglInitialize(eglDisplay, &major, &minor)) {
        loge("eglInitialize() returned error 0x%x", eglGetError());
        return false;
    }

    EGLint configSpec[] = {
            // EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE, 0,
            EGL_NONE
    };

    const int configSize = sizeof(configSpec) / sizeof(*configSpec);

    logw("eglInitialize: major: %d, minor: %d", major, minor);
    switch (type) {
        case RECORDABLE_ANDROID:
            configSpec[configSize - 3] = EGL_RECORDABLE_ANDROID;
            configSpec[configSize - 2] = 1;
            break;
        case PBUFFER:
            configSpec[configSize - 3] = EGL_SURFACE_TYPE;
            configSpec[configSize - 2] = EGL_PBUFFER_BIT;

        default:;
    }
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(eglDisplay, configSpec, &config, 1, &numConfigs)) {
        loge("eglChooseConfig() returned error 0x%x", eglGetError());
        destroy();
        return false;
    }

    logw("Config num: %d, sharedContext id: %p", numConfigs, eglContext);

    EGLint attribList[]{
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    eglContext = eglCreateContext(eglDisplay, config, eglContext, attribList);
    if (eglContext == EGL_NO_CONTEXT) {
        loge("eglCreateContext Failed:  0x%x", eglGetError());
        return false;
    }

    const EGLint pBufferAttrib[] = {
            EGL_WIDTH, width,
            EGL_HEIGHT, height,
            EGL_NONE
    };

    eglSurface = eglCreatePbufferSurface(eglDisplay, config, pBufferAttrib);
    if (eglSurface == EGL_NO_SURFACE) {
        loge("eglCreatePbufferSurface Failed:  0x%x", eglGetError());
        return false;
    }

    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        loge("eglMakeCurrent failed: 0x%x", eglGetError());
        return false;
    }

    int clientVersion;
    eglQueryContext(eglDisplay, eglContext, EGL_CONTEXT_CLIENT_VERSION, &clientVersion);
    logw("EGLContext created, client version %d\n", clientVersion);

    return true;
}

void CGESharedGLContext::destroy() {
    logi("####CGESharedGLContext Destroying context... ####");

    if (eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(eglDisplay, eglContext);
        eglDestroySurface(eglDisplay, eglSurface);
        eglTerminate(eglDisplay);
    }

    eglDisplay = EGL_NO_DISPLAY;
    eglSurface = EGL_NO_SURFACE;
    eglContext = EGL_NO_CONTEXT;
}

void CGESharedGLContext::makecurrent() {
    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        loge("eglMakeCurrent failed: 0x%x", eglGetError());
    }
}

bool CGESharedGLContext::swapbuffers() {
    return eglSwapBuffers(eglDisplay, eglSurface);
}
