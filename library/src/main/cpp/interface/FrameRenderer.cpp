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

    FrameRenderer::FrameRenderer() : textureDrawer(nullptr), textureDrawerExtOES(nullptr),
                                     cacheDrawer(nullptr), m_isUsingMask(false),
                                     m_drawerFlipScaleX(1.0f), m_drawerFlipScaleY(1.0f),
                                     fastFrameHandler(nullptr) {

    }


    FrameRenderer::~FrameRenderer() {

        delete textureDrawer;
        delete textureDrawerExtOES;

        delete cacheDrawer;
        cacheDrawer = nullptr;

        delete fastFrameHandler;
        fastFrameHandler = nullptr;
    }

    bool FrameRenderer::init(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
        loge("src w %d h %d dest w %d h %d",srcWidth,srcHeight,dstWidth,dstHeight);
        srcSize.set(srcWidth, srcHeight);
        dstSize.set(dstWidth, dstHeight);

        //有可能通过预先设置为 mask版
        if (textureDrawer == nullptr)
            textureDrawer = TextureDrawer::create();

//        class TextureDrawer4ExtOES : public TextureDrawer
        if (textureDrawerExtOES == nullptr)
            textureDrawerExtOES = TextureDrawer4ExtOES::create();


        if (cacheDrawer == nullptr)
            cacheDrawer = TextureDrawer::create();

        calcViewport(srcWidth, srcHeight, dstWidth, dstHeight);

//        class ImageHandler : public ImageHandlerInterface
//        class FastFrameHandler : public ImageHandler
        if (fastFrameHandler == nullptr) {
            fastFrameHandler = new FastFrameHandler();
        }

        return textureDrawer != nullptr && textureDrawerExtOES != nullptr &&
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
        float scaling = CGE_MAX(dstWidth / (float) srcWidth, dstHeight / (float) srcHeight);

        if (scaling != 0.0f) {
            float sw = srcWidth * scaling, sh = srcHeight * scaling;
            viewport[0] = (dstWidth - sw) / 2.0f;
            viewport[1] = (dstHeight - sh) / 2.0f;
            viewport[2] = sw;
            viewport[3] = sh;
            logi("CGEFrameRenderer - viewport: %d, %d, %d, %d", viewport[0],
                 viewport[1], viewport[2], viewport[3]);
        }
    }

    void FrameRenderer::update(GLuint externalTexture, float *transformMatrix) {
        CHECK_RENDERER_STATUS;

        fastFrameHandler->useImageFBO();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        textureDrawerExtOES->setTransform(transformMatrix);
        textureDrawerExtOES->drawTexture(externalTexture);
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
        textureDrawer->drawTexture(fastFrameHandler->getTargetTextureID());
    }

    void FrameRenderer::drawCache() {
        CHECK_RENDERER_STATUS;

        cacheDrawer->drawTexture(fastFrameHandler->getTargetTextureID());
    }

    void FrameRenderer::setSrcRotation(float rad) {
        textureDrawerExtOES->setRotation(rad);
    }

    void FrameRenderer::setSrcFlipScale(float x, float y) {
        textureDrawerExtOES->setFlipScale(x, y);
    }

    void FrameRenderer::setRenderRotation(float rad) {
        textureDrawer->setRotation(rad);
    }

    void FrameRenderer::setRenderFlipScale(float x, float y) {
        m_drawerFlipScaleX = x;
        m_drawerFlipScaleY = y;
        textureDrawer->setFlipScale(x, y);
    }

    void FrameRenderer::setFilterWithConfig(CGEConstString config, CGETextureLoadFun texLoadFunc,
                                            void *loadArg) {
        CHECK_RENDERER_STATUS;

        std::unique_lock<std::mutex> uniqueLock(resultMutex);

        if (config == nullptr || *config == '\0') {
            fastFrameHandler->clearImageFilters(true);
            return;
        }

        CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter;
        filter->setTextureLoadFunction(texLoadFunc, loadArg);

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
            if (m_isUsingMask || textureDrawer == nullptr) {
                m_isUsingMask = false;
                delete textureDrawer;
                textureDrawer = TextureDrawer::create();
                textureDrawer->setFlipScale(m_drawerFlipScaleX, m_drawerFlipScaleY);
            }
            return;
        }

        m_isUsingMask = true;

        TextureDrawerWithMask *drawer = TextureDrawerWithMask::create();
        if (drawer == nullptr) {
            loge("init drawer program failed!");
            return;
        }

        delete textureDrawer;
        textureDrawer = drawer;
        drawer->setMaskTexture(maskTexture);
        setMaskTextureRatio(aspectRatio);
    }

    void FrameRenderer::setMaskTextureRatio(float aspectRatio) {
        float dstRatio = dstSize.width / (float) dstSize.height;
        float s = dstRatio / aspectRatio;
        if (s > 1.0f) {
            textureDrawer->setFlipScale(m_drawerFlipScaleX / s, m_drawerFlipScaleY);
        } else {
            textureDrawer->setFlipScale(m_drawerFlipScaleX, s * m_drawerFlipScaleY);
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
        if (!m_isUsingMask || textureDrawer == nullptr)
            return;

        TextureDrawerWithMask *drawer = dynamic_cast<TextureDrawerWithMask *>(textureDrawer);
        assert(drawer != nullptr);
        drawer->setMaskRotation(rad);
    }

    void FrameRenderer::setMaskFlipScale(float x, float y) {
        if (!m_isUsingMask || textureDrawer == nullptr)
            return;

        TextureDrawerWithMask *drawer = dynamic_cast<TextureDrawerWithMask *>(textureDrawer);
        assert(drawer != nullptr);
        drawer->setMaskFlipScale(x, y);
    }

}












