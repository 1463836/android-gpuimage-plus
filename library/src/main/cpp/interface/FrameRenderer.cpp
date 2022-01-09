/*
* cgeFrameRenderer.cpp
*
*  Created on: 2015-11-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "FrameRenderer.h"
#include "MultipleEffects.h"

#include <chrono>

#if defined(DEBUG) || defined(_DEBUG)

#define CHECK_RENDERER_STATUS \
if(fastFrameHandler == nullptr)\
{\
    loge("FrameRenderer is not initialized!!\n");\
    return;\
}

#else

#define CHECK_RENDERER_STATUS

#endif

namespace CGE {
    void FastFrameHandler::processingFilters() {
        if (vecFilters.empty() || fboTextures[0] == 0) {
            glFlush();
            return;
        }

        glDisable(GL_BLEND);
        assert(vertexArrayBuffer != 0);

        glViewport(0, 0, dstImageSize.width, dstImageSize.height);

        for (std::vector<ImageFilterInterfaceAbstract *>::iterator iter = vecFilters.begin();
             iter < vecFilters.end(); ++iter) {
            swapBufferFBO();
            glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
            (*iter)->render2Texture(this, fboTextures[1], vertexArrayBuffer);
            glFlush();
        }
        glFinish();
        // glFlush();
    }

    void FastFrameHandler::swapBufferFBO() {
        useImageFBO();
        std::swap(fboTextures[0], fboTextures[1]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               fboTextures[0], 0);

        //为了效率， 高帧率绘制期间不检测错误.
    }


    //////////////////////////////////////////////

    FrameRenderer::FrameRenderer() : textureDrawBase(nullptr), textureDrawOES(nullptr),
                                     textureDrawCache(nullptr), m_isUsingMask(false),
                                     m_drawerFlipScaleX(1.0f), m_drawerFlipScaleY(1.0f),
                                     fastFrameHandler(nullptr) {

    }


    FrameRenderer::~FrameRenderer() {

        delete textureDrawBase;
        delete textureDrawOES;

        delete textureDrawCache;
        textureDrawCache = nullptr;

        delete fastFrameHandler;
        fastFrameHandler = nullptr;
    }

    bool FrameRenderer::init(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
//        src w 480 h 640 dest w 480 h 640
        loge("src w %d h %d dest w %d h %d", srcWidth, srcHeight, dstWidth, dstHeight);
        srcSize.set(srcWidth, srcHeight);
        dstSize.set(dstWidth, dstHeight);

        //有可能通过预先设置为 mask版
        if (textureDrawBase == nullptr)
            textureDrawBase = TextureDrawer::create();

//        class TextureDrawer4ExtOES : public TextureDrawer
        if (textureDrawOES == nullptr)
            textureDrawOES = TextureDrawer4ExtOES::create();


        if (textureDrawCache == nullptr)
            textureDrawCache = TextureDrawer::create();

        calcViewport(srcWidth, srcHeight, dstWidth, dstHeight);

//        class ImageHandler : public ImageHandlerInterface
//        class FastFrameHandler : public ImageHandler
        if (fastFrameHandler == nullptr) {
            fastFrameHandler = new FastFrameHandler();
        }

        return textureDrawBase != nullptr && textureDrawOES != nullptr &&
               fastFrameHandler->initWithRawBufferData(nullptr, dstWidth, dstHeight,
                                                       CGE_FORMAT_RGBA_INT8, false);
    }

    void FrameRenderer::srcResize(int width, int height) {
        CHECK_RENDERER_STATUS;

        logi("srcResize: %d, %d", width, height);
        Sizei sz = fastFrameHandler->getOutputFBOSize();
        if (sz.width <= 0 || sz.height <= 0) {
            loge("CGEFrameRenderer not initialized!\n");
            return;
        }
        calcViewport(width, height, sz.width, sz.height);
    }

    void FrameRenderer::calcViewport(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
//        src w 480 h 640 dest w 480 h 640

        float scaling = CGE_MAX(dstWidth / (float) srcWidth, dstHeight / (float) srcHeight);

        if (scaling != 0.0f) {
            float sw = srcWidth * scaling,
                    sh = srcHeight * scaling;
            viewport[0] = (dstWidth - sw) / 2.0f;
            viewport[1] = (dstHeight - sh) / 2.0f;
            viewport[2] = sw;
            viewport[3] = sh;
            logw("FrameRenderer - viewport: %d, %d, %d, %d", viewport[0],
                 viewport[1], viewport[2], viewport[3]);

        }
    }

    void FrameRenderer::update(GLuint textureOES, float *matrix) {
        CHECK_RENDERER_STATUS;

        fastFrameHandler->useImageFBO();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        textureDrawOES->setTransform(matrix);
        textureDrawOES->drawTexture(textureOES);
    }

    void FrameRenderer::runProc() {
        CHECK_RENDERER_STATUS;
        resultMutex.lock();
        fastFrameHandler->processingFilters();
        resultMutex.unlock();
    }

    void FrameRenderer::render(int x, int y, int width, int height) {
        CHECK_RENDERER_STATUS;
        glViewport(x, y, width, height);
        textureDrawBase->drawTexture(fastFrameHandler->getTargetTextureID());
    }

    void FrameRenderer::drawCache() {
        CHECK_RENDERER_STATUS;

        textureDrawCache->drawTexture(fastFrameHandler->getTargetTextureID());
    }

    void FrameRenderer::setSrcRotation(float rad) {
        textureDrawOES->setRotation(rad);
    }

    void FrameRenderer::setSrcFlipScale(float x, float y) {
        textureDrawOES->setFlipScale(x, y);
    }

    void FrameRenderer::setRenderRotation(float rad) {
        textureDrawBase->setRotation(rad);
    }

    void FrameRenderer::setRenderFlipScale(float x, float y) {
        m_drawerFlipScaleX = x;
        m_drawerFlipScaleY = y;
        textureDrawBase->setFlipScale(x, y);
    }

//    cgeGlobalTextureLoadFunc             CGETexLoadArg
    void FrameRenderer::setFilterWithConfig(ConstString config, CGETextureLoadFun cgeGlobalTextureLoadFunc,
                                            void *cgeTexLoadArg) {
        CHECK_RENDERER_STATUS;

        logw("%s %s",__FUNCTION__ ,config);
        std::unique_lock<std::mutex> uniqueLock(resultMutex);

        if (config == nullptr || *config == '\0') {
            fastFrameHandler->clearImageFilters(true);
            return;
        }

        CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter;
        filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, cgeTexLoadArg);

        if (!filter->initWithEffectString(config)) {
            delete filter;
            return;
        }

        filter->setIntensity(1.0f);
        fastFrameHandler->clearImageFilters(true);
        fastFrameHandler->addImageFilter(filter);
    }

    void FrameRenderer::setFilter(ImageFilterInterfaceAbstract *filter) {
        CHECK_RENDERER_STATUS;

        std::unique_lock<std::mutex> uniqueLock(resultMutex);
        fastFrameHandler->clearImageFilters(true);
        fastFrameHandler->addImageFilter(filter);
    }

    void FrameRenderer::setFilterIntensity(float value) {
        CHECK_RENDERER_STATUS;

        resultMutex.lock();
        auto &filters = fastFrameHandler->peekFilters();
        for (auto &filter : filters) {
            filter->setIntensity(value);
        }
        resultMutex.unlock();
    }

    void FrameRenderer::setMaskTexture(GLuint maskTexture, float aspectRatio) {
        if (maskTexture == 0) {
            if (m_isUsingMask || textureDrawBase == nullptr) {
                m_isUsingMask = false;
                delete textureDrawBase;
                textureDrawBase = TextureDrawer::create();
                textureDrawBase->setFlipScale(m_drawerFlipScaleX, m_drawerFlipScaleY);
            }
            return;
        }

        m_isUsingMask = true;

        TextureDrawerWithMask *drawer = TextureDrawerWithMask::create();
        if (drawer == nullptr) {
            loge("init drawer program failed!");
            return;
        }

        delete textureDrawBase;
        textureDrawBase = drawer;
        drawer->setMaskTexture(maskTexture);
        setMaskTextureRatio(aspectRatio);
    }

    void FrameRenderer::setMaskTextureRatio(float aspectRatio) {
        float dstRatio = dstSize.width / (float) dstSize.height;
        float s = dstRatio / aspectRatio;
        if (s > 1.0f) {
            textureDrawBase->setFlipScale(m_drawerFlipScaleX / s, m_drawerFlipScaleY);
        } else {
            textureDrawBase->setFlipScale(m_drawerFlipScaleX, s * m_drawerFlipScaleY);
        }
    }

    GLuint FrameRenderer::getTargetTexture() {
        if (fastFrameHandler == nullptr) {
            loge("CGEFrameRenderer is not initialized!!\n");
            return 0;
        }

        return fastFrameHandler->getTargetTextureID();
    }

    GLuint FrameRenderer::getBufferTexture() {
        if (fastFrameHandler == nullptr) {
            loge("CGEFrameRenderer is not initialized!!\n");
            return 0;
        }

        return fastFrameHandler->getBufferTextureID();
    }

    void FrameRenderer::setMaskRotation(float rad) {
        if (!m_isUsingMask || textureDrawBase == nullptr)
            return;

        TextureDrawerWithMask *drawer = dynamic_cast<TextureDrawerWithMask *>(textureDrawBase);
        assert(drawer != nullptr);
        drawer->setMaskRotation(rad);
    }

    void FrameRenderer::setMaskFlipScale(float x, float y) {
        if (!m_isUsingMask || textureDrawBase == nullptr)
            return;

        TextureDrawerWithMask *drawer = dynamic_cast<TextureDrawerWithMask *>(textureDrawBase);
        assert(drawer != nullptr);
        drawer->setMaskFlipScale(x, y);
    }

}












