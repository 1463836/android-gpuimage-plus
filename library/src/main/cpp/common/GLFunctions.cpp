﻿/*
* cgeGLFunctions.cpp
*
*  Created on: 2013-12-5
*      Author: Wang Yang
*/

#include "GLFunctions.h"
#include <cmath>

CGE_LOG_CODE
(
        static int sTextureCount;

)

namespace CGE {

#if !(defined(_CGE_DISABLE_GLOBALCONTEXT_) && _CGE_DISABLE_GLOBALCONTEXT_)

    static EnableGLContextFunction enableGLFunc = nullptr;
    static DisableGLContextFunction disableGLFunc = nullptr;
    static void *enableGLParam;
    static void *disableGLParam;
    static bool stopGlobalGLEnableFunc = false;

    void cgeSetGLContextEnableFunction(EnableGLContextFunction func, void *param) {
        enableGLFunc = func;
        enableGLParam = param;
    }

    void cgeSetGLContextDisableFunction(DisableGLContextFunction func, void *param) {
        disableGLFunc = func;
        disableGLParam = param;
    }

    void *cgeGetGLEnableParam() {
        return enableGLParam;
    }

    void *cgeGetGLDisableParam() {
        return disableGLParam;
    }

    void cgeStopGlobalGLEnableFunction() {
        stopGlobalGLEnableFunc = true;
    }

    void cgeRestoreGlobalGLEnableFunction() {
        stopGlobalGLEnableFunc = false;
    }

    void enableGlobalGLContext() {
        if (enableGLFunc)
            enableGLFunc(enableGLParam);
    }

    void cgeDisableGlobalGLContext() {
        if (disableGLFunc)
            disableGLFunc(disableGLParam);
    }

#endif

    //////////////////////////////////////////////////////////////////////////

    static CGEBufferLoadFun s_loadDataWithSourceNameCommon = nullptr;
    static CGEBufferUnloadFun s_unloadBufferDataCommon = nullptr;
    static void *s_cgeLoadParam = nullptr;
    static void *s_cgeUnloadParam = nullptr;


    void cgeSetCommonLoadFunction(CGEBufferLoadFun fun, void *arg) {
        s_loadDataWithSourceNameCommon = fun;
        s_cgeLoadParam = arg;

    }

    void cgeSetCommonUnloadFunction(CGEBufferUnloadFun fun, void *arg) {
        s_unloadBufferDataCommon = fun;
        s_cgeUnloadParam = arg;
    }

    void *
    cgeLoadResourceCommon(const char *sourceName, void **bufferData, CGEBufferFormat *fmt, GLint *w,
                          GLint *h) {
        if (s_loadDataWithSourceNameCommon != nullptr)
            return s_loadDataWithSourceNameCommon(sourceName, bufferData, w, h, fmt,
                                                  &s_cgeLoadParam);
        return nullptr;
    }

    bool cgeUnloadResourceCommon(void *bufferArg) {
        if (s_unloadBufferDataCommon != nullptr)
            return s_unloadBufferDataCommon(bufferArg, s_cgeUnloadParam);
        return false;
    }

    CGEBufferLoadFun cgeGetCommonLoadFunc() {
        return s_loadDataWithSourceNameCommon;
    }

    CGEBufferUnloadFun cgeGetCommonUnloadFunc() {
        return s_unloadBufferDataCommon;
    }

    void *cgeGetCommonLoadArg() {
        return s_cgeLoadParam;
    }

    void *cgeGetCommonUnloadArg() {
        return s_cgeUnloadParam;
    }

    char *cgeGetScaledBufferOutofSize(const void *buffer, int &w, int &h, int channel, int minSizeX,
                                      int minSizeY) {
        if ((minSizeX < w && minSizeY < h) || buffer == nullptr) return nullptr;
        char *tmpbuffer = nullptr;
        const char *data = (const char *) buffer;

        int width = w;
        double scale = CGE_MIN(w / (float) minSizeX, h / (float) minSizeY);
        w = ceilf(w / scale);
        h = ceilf(h / scale);

        int len = w * h;
        tmpbuffer = new char[len * channel];
        if (channel == 4) {
            for (int i = 0; i != h; ++i) {
                for (int j = 0; j != w; ++j) {
                    const int L = (j + i * w) * channel;
                    const int R =
                            (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) *
                            channel;
                    tmpbuffer[L] = data[R];
                    tmpbuffer[L + 1] = data[R + 1];
                    tmpbuffer[L + 2] = data[R + 2];
                    tmpbuffer[L + 3] = data[R + 3];
                }
            }
        } else if (channel == 3) {
            for (int i = 0; i != h; ++i) {
                for (int j = 0; j != w; ++j) {
                    const int L = (j + i * w) * channel;
                    const int R =
                            (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) *
                            channel;
                    tmpbuffer[L] = data[R];
                    tmpbuffer[L + 1] = data[R + 1];
                    tmpbuffer[L + 2] = data[R + 2];
                }
            }
        }
        return tmpbuffer;
    }

    char *getScaledBufferInSize(const void *buffer, int &w, int &h, int channel, int maxSizeX,
                                int maxSizeY) {
        if ((maxSizeX > w && maxSizeY > h) || buffer == nullptr) return nullptr;
        char *tmpbuffer = nullptr;
        const char *data = (const char *) buffer;

        int width = w;
        double scale = CGE_MAX(w / (float) maxSizeX, h / (float) maxSizeY);
        w = w / scale;
        h = h / scale;
        int len = w * h;
        tmpbuffer = new char[len * channel];
        if (channel == 4) {
            for (int i = 0; i != h; ++i) {
                for (int j = 0; j != w; ++j) {
                    const int L = (j + i * w) * channel;
                    const int R =
                            (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) *
                            channel;
                    tmpbuffer[L] = data[R];
                    tmpbuffer[L + 1] = data[R + 1];
                    tmpbuffer[L + 2] = data[R + 2];
                    tmpbuffer[L + 3] = data[R + 3];
                }
            }
        } else if (channel == 3) {
            for (int i = 0; i != h; ++i) {
                for (int j = 0; j != w; ++j) {
                    const int L = (j + i * w) * channel;
                    const int R =
                            (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) *
                            channel;
                    tmpbuffer[L] = data[R];
                    tmpbuffer[L + 1] = data[R + 1];
                    tmpbuffer[L + 2] = data[R + 2];
                }
            }
        }
        return tmpbuffer;
    }

    /////////////////////////////////////////////////

    SharedTexture::SharedTexture(GLuint textureID, int w, int h) {
        m_textureID = textureID;
        m_refCount = new int(1);
        width = w;
        height = h;
        CGE_LOG_CODE
        (
                if (m_textureID == 0)
                    loge("CGESharedTexture : Invalid TextureID!");
                else {
                    logi("---CGESharedTexture creating, textureID %d, total : %d ###\n",
                         textureID,
                         ++sTextureCount);
                }
        );
    }

    SharedTexture::~SharedTexture() {
        if (m_refCount == nullptr) {
            CGE_LOG_CODE
            (
                    if (m_textureID != 0) {
                        loge("SharedTexture : Error occurred!");
                    });
            return;
        }

        --*m_refCount;
        if (*m_refCount <= 0) {
            clear();
        }
        CGE_LOG_CODE
        (
                else
                {
                    logi("@@@ Texture %d deRef count: %d\n", m_textureID, *m_refCount);
                })
    }

    void SharedTexture::forceRelease(bool bDelTexture) {
        assert(m_refCount == nullptr ||
               *m_refCount == 1); // 使用 forceRelease 时 SharedTexture 必须保证只存在一个实例
        if (bDelTexture)
            glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
        CGE_DELETE(m_refCount);
        width = 0;
        height = 0;
        CGE_LOG_CODE
        (
                --sTextureCount;
        );
    }

    void SharedTexture::clear() {
        CGE_LOG_CODE
        (
                if (m_textureID == 0) {
                    loge("!!!CGESharedTexture - Invalid TextureID To Release!\n");
                } else {
                    logi("###CGESharedTexture deleting, textureID %d, now total : %d ###\n",
                         m_textureID, --sTextureCount);
                });

        assert(*m_refCount == 0); // 未知错误

        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;

        CGE_DELETE(m_refCount);
        width = 0;
        height = 0;
    }
}








