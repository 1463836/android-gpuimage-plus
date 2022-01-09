/*
* cgeImageHandler.cpp
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "ImageHandler.h"
#include "TextureUtils.h"

#include <cassert>

CGE_UNEXPECTED_ERR_MSG
(
        static int sHandlerCount = 0;

)

namespace CGE {
    ImageHandlerInterface::ImageHandlerInterface() : textureSrc(0), fbo(0),
                                                     vertexArrayBuffer(0) {
        dstImageSize.set(0, 0);
        fboTextures[0] = 0;
        fboTextures[1] = 0;

        CGE_UNEXPECTED_ERR_MSG
        (
                CGE_LOG_KEEP("Handler create, total: %d\n", ++sHandlerCount);
        )
    }

    ImageHandlerInterface::~ImageHandlerInterface() {
        CGE_ENABLE_GLOBAL_GLCONTEXT();
        glDeleteTextures(1, &textureSrc);
        clearImageFBO();
        glDeleteBuffers(1, &vertexArrayBuffer);
        vertexArrayBuffer = 0;

        CGE_UNEXPECTED_ERR_MSG
        (
                CGE_LOG_KEEP("Handler release, remain: %d\n", --sHandlerCount);
        )
    }

    GLuint ImageHandlerInterface::getResultTextureAndClearHandler() {
        CGE_ENABLE_GLOBAL_GLCONTEXT();
        glFinish();
        GLuint texID = fboTextures[0];
        fboTextures[0] = 0;
        clearImageFBO();
        glDeleteTextures(1, &textureSrc);
        textureSrc = 0;
        return texID;
    }

    size_t ImageHandlerInterface::getOutputBufferLen(size_t channel) {
        return dstImageSize.width * dstImageSize.height * channel;
    }

    size_t ImageHandlerInterface::getOutputBufferBytesPerRow(size_t channel) {
        return dstImageSize.width * channel;
    }

    void ImageHandlerInterface::copyTextureData(void *data, int w, int h, GLuint texID,
                                                GLenum dataFmt, GLenum channelFmt) {
        assert(texID != 0); //Invalid Texture ID

        CGE_ENABLE_GLOBAL_GLCONTEXT();

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
        glFinish();
        if (channelFmt != GL_RGBA)
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, w, h, channelFmt, dataFmt, data);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               fboTextures[0], 0);
        cgeCheckGLError("CGEImageHandlerInterface::copyTextureData");
    }


    bool
    ImageHandlerInterface::initImageFBO(const void *bufferdata, int dstWidth, int dstHeight, GLenum channelFmt,
                                        GLenum dataFmt, int channel) {
        clearImageFBO();

        if (vertexArrayBuffer == 0)
            vertexArrayBuffer = genCommonQuadArrayBuffer();
        logi("Vertex Array Buffer id: %d\n", vertexArrayBuffer);


        //        bufferdata =null;
        //        w 480 h 640
        //        channelFmt = GL_RGBA;
        //        dataFmt = GL_UNSIGNED_BYTE;
        //        channel = 4;

        //Set the swap buffer textures.
        fboTextures[0] = genTextureWithBuffer(bufferdata, dstWidth, dstHeight, channelFmt, dataFmt, channel);
        fboTextures[1] = genTextureWithBuffer(nullptr, dstWidth, dstHeight, channelFmt, dataFmt, channel);

        logi("FBO buffer texture id: %d and %d\n", fboTextures[0], fboTextures[1]);

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               fboTextures[0], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            clearImageFBO();
            loge("Image Handler initImageFBO failed! %x\n",
                 glCheckFramebufferStatus(GL_FRAMEBUFFER));
            cgeCheckGLError("CGEImageHandlerInterface::initImageFBO");
            return false;
        }
        logi("FBO Framebuffer id: %d\n", fbo);
        return true;
    }

    void ImageHandlerInterface::clearImageFBO() {
//        CGE_ENABLE_GLOBAL_GLCONTEXT();
        enableGlobalGLContext();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(2, fboTextures);
        fboTextures[0] = 0;
        fboTextures[1] = 0;
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }

    //////////////////////////////////////////////////////////////////////////

    ImageHandler::ImageHandler() : revertEnabled(false), drawer(nullptr),
                                   resultDrawer(nullptr)
#ifdef _CGE_USE_ES_API_3_0_
    ,m_pixelPackBuffer(0), m_pixelPackBufferSize(0)
#endif
    {}

    ImageHandler::~ImageHandler() {
        CGE_ENABLE_GLOBAL_GLCONTEXT();
        clearImageFilters();
        delete drawer;
        delete resultDrawer;

#ifdef _CGE_USE_ES_API_3_0_
        clearPixelBuffer();
#endif

    }



    bool ImageHandler::initWithRawBufferData(const void *imgData, GLint dstWidth, GLint dstHeight,
                                             CGEBufferFormat format, bool enableReversion) {
//        nullptr, dstWidth, dstHeight,CGE_FORMAT_RGBA_INT8, false

//        CGE_ENABLE_GLOBAL_GLCONTEXT();
        enableGlobalGLContext();
        int channel;
        GLenum dataFmt, channelFmt;
        getDataAndChannelByFormat(format, &dataFmt, &channelFmt, &channel);

        loge(" %d x %d, format %d dataFmt %x channelFmt %x channel %d", dstWidth, dstHeight, format, dataFmt, channelFmt, channel);
//        480 x 640,
//        dataFmt = GL_UNSIGNED_BYTE;
//        channelFmt = GL_RGBA;
//        channel = 4;

//        imgData=null;
        if (channel == 0)
            return false;
        char *tmpbuffer = getScaledBufferInSize(imgData, dstWidth, dstHeight, channel, getMaxTextureSize(),
                                                getMaxTextureSize());
//        logw("imgData=null %d",(imgData==NULL));
//        tmpbuffer=null;
        const char *bufferdata = (tmpbuffer == nullptr) ? (const char *) imgData : tmpbuffer;
//        bufferdata =null;
        dstImageSize.set(dstWidth, dstHeight);
//        logi("Image Handler Init With RawBufferData %d x %d, %d channel\n", w, h, channel);

        glDeleteTextures(1, &textureSrc); //Delete last texture to avoid reinit error.
        revertEnabled = enableReversion;//false
        if (revertEnabled) {
            textureSrc = genTextureWithBuffer(bufferdata, dstWidth, dstHeight, channelFmt, dataFmt, channel);
            logi("Input Image Texture id %d\n", textureSrc);
        } else {
            textureSrc = 0;
        }

        bool status = initImageFBO(bufferdata, dstWidth, dstHeight, channelFmt, dataFmt, channel);

        delete[] tmpbuffer;

        cgeCheckGLError("CGEImageHandler::initWithRawBufferData");
        return status;
    }

    bool ImageHandler::updateData(const void *data, int w, int h, CGEBufferFormat format) {
        int channel;
        GLenum dataFmt, channelFmt;
        getDataAndChannelByFormat(format, &dataFmt, &channelFmt, &channel);
        if (!(w == dstImageSize.width && h == dstImageSize.height && channel == 4))
            return false;

        CGE_ENABLE_GLOBAL_GLCONTEXT();

        glBindTexture(GL_TEXTURE_2D, fboTextures[0]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, channelFmt, dataFmt, data);
        return true;
    }

    bool
    ImageHandler::initWithTexture(GLuint textureID, GLint w, GLint h, CGEBufferFormat format,
                                  bool bEnableReversion) {
        if (textureID == 0 || w < 1 || h < 1)
            return false;

        textureSrc = textureID;
        dstImageSize.set(w, h);

        GLenum dataFmt, channelFmt;
        GLint channel;
        getDataAndChannelByFormat(format, &dataFmt, &channelFmt, &channel);

        initImageFBO(nullptr, w, h, channelFmt, dataFmt, channel);

        revertEnabled = true;
        revertToKeptResult(false);
        revertEnabled = bEnableReversion;

        if (!revertEnabled)
            textureSrc = 0;
        return true;
    }

#ifdef _CGE_USE_ES_API_3_0_
    extern bool g_shouldUsePBO;

    bool ImageHandler::initPixelBuffer()
    {
        cgeCheckGLError("before ImageHandlerInterface::initPixelBuffer");

        bool ret = false;

        if(g_shouldUsePBO)
        {
            glDeleteBuffers(1, &m_pixelPackBuffer);
            glGenBuffers(1, &m_pixelPackBuffer);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pixelPackBuffer);

            GLenum err = glGetError();

            if(err == GL_FALSE && m_pixelPackBuffer != 0 && m_pixelPackBufferSize != 0)
            {
                glBufferData(GL_PIXEL_PACK_BUFFER, m_pixelPackBufferSize, 0, GL_DYNAMIC_READ);
                ret = true;
            }
            else
            {
                g_shouldUsePBO = false;
                glDeleteBuffers(1, &m_pixelPackBuffer);
                m_pixelPackBuffer = 0;
                m_pixelPackBuffer = 0;
                m_pixelPackBufferSize = 0;
                loge("GL_PIXEL_PACK_BUFFER - failed! Error code %x\n", err);
            }

            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        }

        return ret;
    }

    bool ImageHandler::initImageFBO(const void* data, int w, int h, GLenum channelFmt, GLenum dataFmt, int channel)
    {
        m_pixelPackBufferSize = dstImageSize.width * dstImageSize.height * channel;
        initPixelBuffer();
        return ImageHandlerInterface::initImageFBO(data, w, h, channelFmt, dataFmt, channel);
    }

    const void* ImageHandler::mapOutputBuffer(CGEBufferFormat fmt)
    {
        if(!g_shouldUsePBO || m_pixelPackBuffer == 0)
            return nullptr;

        int channel;
        GLenum channelFmt, dataFmt;
        cgeGetDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, &channel);

        if(m_pixelPackBufferSize != dstImageSize.width * dstImageSize.height * channel)
        {
            loge("Invalid format!\n");
            return nullptr;
        }

        CGE_ENABLE_GLOBAL_GLCONTEXT();
        useImageFBO();
        glFinish();

        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pixelPackBuffer);
        glReadPixels(0, 0, dstImageSize.width, dstImageSize.height, channelFmt, dataFmt, 0);
        const void* ret = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_pixelPackBufferSize, GL_MAP_READ_BIT);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        return ret;
    }

    void ImageHandler::unmapOutputBuffer()
    {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pixelPackBuffer);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

#endif

    bool ImageHandler::getOutputBufferData(void *data, CGEBufferFormat fmt) {
        int channel;
        GLenum channelFmt, dataFmt;
        getDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, &channel);
        size_t len = getOutputBufferLen(channel);
        if (nullptr == data || len == 0 || channel != 4) {
            loge("%s\n", nullptr == data ? "data is NULL" : (channel == 4
                                                             ? "Handler is not initialized!"
                                                             : "Channel must be 4!"));
            return false;
        }
        CGE_ENABLE_GLOBAL_GLCONTEXT();
        setAsTarget();
        glFinish();
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

#ifdef _CGE_USE_ES_API_3_0_

        cgeCheckGLError("ImageHandlerInterface::ImageHandlerInterface");

        if(g_shouldUsePBO)
        {
            if(m_pixelPackBuffer == 0 || m_pixelPackBufferSize != dstImageSize.width * dstImageSize.height * channel)
            {
                m_pixelPackBufferSize = dstImageSize.width * dstImageSize.height * channel;
                initPixelBuffer();
            }

            if(m_pixelPackBuffer != 0)
            {
                glReadBuffer(GL_COLOR_ATTACHMENT0);
                glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pixelPackBuffer);
                glReadPixels(0, 0, dstImageSize.width, dstImageSize.height, channelFmt, dataFmt, 0);
                GLubyte* bytes =(GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_pixelPackBufferSize, GL_MAP_READ_BIT);

                if(bytes != nullptr)
                {
                    memcpy(data, bytes, m_pixelPackBufferSize);
                }
                else
                {
                    loge("glMapBufferRange failed! Use normal read pixels instead...\n");
                    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
                    glReadPixels(0, 0, dstImageSize.width, dstImageSize.height, channelFmt, dataFmt, data);
                }
                glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
                glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            }
        }
        else
        {
            glReadPixels(0, 0, dstImageSize.width, dstImageSize.height, channelFmt, dataFmt, data);
        }

#else
        glReadPixels(0, 0, dstImageSize.width, dstImageSize.height, channelFmt, dataFmt, data);
#endif
        return true;
    }

#ifdef _CGE_USE_ES_API_3_0_

    void ImageHandler::clearPixelBuffer()
    {
        glDeleteBuffers(1, &m_pixelPackBuffer);
        m_pixelPackBuffer = 0;
        m_pixelPackBufferSize = 0;
    }

#endif

    void ImageHandler::setAsTarget() {
        CGE_ENABLE_GLOBAL_GLCONTEXT();
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, dstImageSize.width, dstImageSize.height);
        CGE_LOG_CODE(
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                    loge("CGEImageHandler::setAsTarget failed!\n");
                });
    }

    void ImageHandler::useImageFBO() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    size_t ImageHandler::getOutputBufferLen(size_t channel) {
        if (fboTextures[0] == 0 || fbo == 0)
            return 0;
        return dstImageSize.width * dstImageSize.height * channel;
    }

    size_t ImageHandler::getOutputBufferBytesPerRow(size_t channel) {
        if (fboTextures[0] == 0 || fbo == 0)
            return 0;
        return dstImageSize.width * channel;
    }

    void ImageHandler::swapBufferFBO() {
        useImageFBO();
        std::swap(fboTextures[0], fboTextures[1]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               fboTextures[0], 0);

        CGE_LOG_CODE
        (
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                    loge("Image Handler swapBufferFBO failed!\n");
                } else {
                    logi("Swapping buffer FBO...\n");
                }
        )
    }

    bool ImageHandler::copyTexture(GLuint dst, GLuint src) {
        if (drawer == nullptr) {
            drawer = TextureDrawer::create();
            if (drawer == nullptr) {
                loge("Texture Drawer create failed!");
                return false;
            }
        }

        GLboolean hasBlending = glIsEnabled(GL_BLEND);
        GLboolean hasDepth = glIsEnabled(GL_DEPTH_TEST);

        if (hasBlending)
            glDisable(GL_BLEND);
        if (hasDepth)
            glDisable(GL_DEPTH_TEST);

        useImageFBO();
        glFlush();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst, 0);
        glViewport(0, 0, dstImageSize.width, dstImageSize.height);
        glClear(GL_COLOR_BUFFER_BIT);
        drawer->drawTexture(src);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               fboTextures[0], 0);

        if (hasBlending)
            glEnable(GL_BLEND);
        if (hasDepth)
            glEnable(GL_DEPTH_TEST);

        return true;
    }

    bool ImageHandler::copyTexture(GLuint dst, GLuint src, int x, int y, int w, int h) {
        return copyTexture(dst, src, 0, 0, x, y, w, h);
    }

    bool
    ImageHandler::copyTexture(GLuint dst, GLuint src, int xOffset, int yOffset, int x, int y,
                              int w, int h) {
        assert(dst != 0 && src != 0);
        useImageFBO();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src, 0);
        glBindTexture(GL_TEXTURE_2D, dst);
        glFinish();
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, x, y, w, h);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               fboTextures[0], 0);
        return true; // Always success.
    }

    void ImageHandler::drawResult() {
        if (resultDrawer == nullptr) {
            resultDrawer = TextureDrawer::create();
            if (resultDrawer == nullptr) {
                loge("Create Texture Drawer Failed!\n");
                return;
            }
        }
        resultDrawer->drawTexture(fboTextures[0]);
    }

    TextureDrawer *ImageHandler::getResultDrawer() {
        if (resultDrawer == nullptr)
            resultDrawer = TextureDrawer::create();
        return resultDrawer;
    }

    void ImageHandler::setResultDrawer(TextureDrawer *drawer) {
        if (resultDrawer != nullptr)
            delete resultDrawer;
        resultDrawer = drawer;
    }

    GLuint ImageHandler::copyLastResultTexture(GLuint texID) {
        if (fboTextures[1] == 0 || fbo == 0)
            return texID;
        CGE_ENABLE_GLOBAL_GLCONTEXT();

        if (texID == 0)
            texID = genTextureWithBuffer(nullptr, dstImageSize.width, dstImageSize.height,
                                         GL_RGBA, GL_UNSIGNED_BYTE);

        if (!copyTexture(texID, fboTextures[1])) {
            useImageFBO();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   fboTextures[1], 0);
            glBindTexture(GL_TEXTURE_2D, texID);
            glFinish();
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dstImageSize.width,
                                dstImageSize.height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   fboTextures[0], 0);
        }

        return texID;
    }

    GLuint ImageHandler::copyResultTexture(GLuint texID) {
        if (fboTextures[1] == 0 || fbo == 0)
            return texID;
        CGE_ENABLE_GLOBAL_GLCONTEXT();

        if (texID == 0)
            texID = genTextureWithBuffer(nullptr, dstImageSize.width, dstImageSize.height,
                                         GL_RGBA, GL_UNSIGNED_BYTE);

        if (!copyTexture(texID, fboTextures[0])) {
            useImageFBO();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   fboTextures[0], 0);
            glBindTexture(GL_TEXTURE_2D, texID);
            glFinish();
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dstImageSize.width,
                                dstImageSize.height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   fboTextures[0], 0);
        }

        return texID;
    }

    void ImageHandler::addImageFilter(ImageFilterInterfaceAbstract *proc) {
        if (proc == nullptr) {
            loge("CGEImageHandler: a null filter is sent. Skipping...\n");
            return;
        }
        if (!proc->isWrapper()) {
            vecFilters.push_back(proc);
            return;
        }

        auto &&filters = proc->getFilters(true);
        for (auto filter : filters) {
            vecFilters.push_back(filter);
        }
        delete proc;
    }

    void ImageHandler::clearImageFilters(bool bDelMem) {
        if (bDelMem) {
            CGE_ENABLE_GLOBAL_GLCONTEXT();
            for (std::vector<ImageFilterInterfaceAbstract *>::iterator iter = vecFilters.begin();
                 iter != vecFilters.end(); ++iter) {
                delete *iter;
            }
        }
        vecFilters.clear();
    }

    void ImageHandler::processingFilters() {
        if (vecFilters.empty() || fboTextures[0] == 0) {
            logi("No filter or image to handle\n");
            return;
        }

        CGE_ENABLE_GLOBAL_GLCONTEXT();
        assert(vertexArrayBuffer != 0);

        glDisable(GL_BLEND);

        CGE_LOG_CODE(int
                             index = 0;);
        CGE_LOG_CODE(clock_t
                             total = clock(););
        for (std::vector<ImageFilterInterfaceAbstract *>::iterator iter = vecFilters.begin();
             iter < vecFilters.end(); ++iter) {
            swapBufferFBO();
            CGE_LOG_CODE(clock_t
                                 t = clock();)
            logi("####Start Processing step %d...\n", ++index);
            glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
            (*iter)->render2Texture(this, fboTextures[1], vertexArrayBuffer);
            glFlush();
            logi("####Processing step %d finished. Time: %gs .\n", index,
                 float (clock() -t) / CLOCKS_PER_SEC);
        }
        glFinish();
        logi("####Finished Processing All! Total time: %gs \n",
             float (clock() -total) / CLOCKS_PER_SEC);
    }

    bool ImageHandler::processingWithFilter(GLint index) {
        if (index == -1)
            index = (GLint) vecFilters.size() - 1;
        return processingWithFilter(getFilterByIndex(index));
    }

    bool ImageHandler::processingWithFilter(ImageFilterInterfaceAbstract *proc) {
        if (proc == nullptr)
            return false;

        CGE_ENABLE_GLOBAL_GLCONTEXT();
        assert(vertexArrayBuffer != 0);

        glDisable(GL_BLEND);
        glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
        swapBufferFBO();

//		CGE_LOG_CODE(clock_t t = clock());
//		CGE_LOG_INFO("####Start Processing...");
        proc->render2Texture(this, fboTextures[1], vertexArrayBuffer);
        glFlush();
//		CGE_LOG_INFO("####Finished Processing! Time: %gs \n", float(clock() - t) / CLOCKS_PER_SEC);
        return true;
    }

    void ImageHandler::disableReversion() {
        CGE_ENABLE_GLOBAL_GLCONTEXT();
        glDeleteTextures(1, &textureSrc);
        textureSrc = 0;
        revertEnabled = false;
        logi("Reversion isdisabled");
    }

    bool ImageHandler::keepCurrentResult() {
        if (!revertEnabled || fboTextures[0] == 0 || fbo == 0)
            return false;

        CGE_ENABLE_GLOBAL_GLCONTEXT();
        useImageFBO();

        glBindTexture(GL_TEXTURE_2D, textureSrc);
        glFinish();
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dstImageSize.width,
                            dstImageSize.height);
        return true;
    }

    bool ImageHandler::revertToKeptResult(bool bRevert2Target) {
        if (!revertEnabled || fboTextures[0] == 0 || fbo == 0)
            return false;

        CGE_ENABLE_GLOBAL_GLCONTEXT();
        useImageFBO();

        if (drawer == nullptr) {
            drawer = TextureDrawer::create();
        }

        if (drawer == nullptr) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   textureSrc, 0);
            if (bRevert2Target) {
                glBindTexture(GL_TEXTURE_2D, fboTextures[1]);

                // glCopyTexSubImage2D 会block cpu
                glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dstImageSize.width,
                                    dstImageSize.height);
                glFlush();
            }
            glBindTexture(GL_TEXTURE_2D, fboTextures[0]);
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dstImageSize.width,
                                dstImageSize.height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   fboTextures[0], 0);
        } else {
            glViewport(0, 0, dstImageSize.width, dstImageSize.height);
            if (bRevert2Target) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                       fboTextures[1], 0);
                drawer->drawTexture(textureSrc);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   fboTextures[0], 0);
            drawer->drawTexture(textureSrc);
        }

        return true;
    }

    int ImageHandler::getFilterIndexByAddr(const void *addr) {
        int sz = (int) vecFilters.size();
        for (int i = 0; i != sz; ++i) {
            if (addr == vecFilters[i])
                return i;
        }
        return -1;
    }

    void ImageHandler::peekFilters(std::vector<ImageFilterInterfaceAbstract *> *vTrans) {
        *vTrans = vecFilters;
    }

    void ImageHandler::popImageFilter() {
        if (!vecFilters.empty()) {
            CGE_ENABLE_GLOBAL_GLCONTEXT();
            std::vector<ImageFilterInterfaceAbstract *>::iterator iter = vecFilters.end() - 1;
            delete *iter;
            vecFilters.erase(iter);
        }
    }

    bool ImageHandler::insertFilterAtIndex(ImageFilterInterfaceAbstract *proc, GLuint index) {
        if (index > vecFilters.size()) return false;
        vecFilters.insert(vecFilters.begin() + index, proc);
        return true;
    }

    bool ImageHandler::deleteFilterByAddr(const void *addr, bool bDelMem) {
        if (vecFilters.empty())
            return false;

        for (std::vector<ImageFilterInterfaceAbstract *>::iterator iter = vecFilters.begin();
             iter < vecFilters.end(); ++iter) {
            if (*iter == addr) {
                if (bDelMem) {
                    CGE_ENABLE_GLOBAL_GLCONTEXT();
                    delete *iter;
                }
                vecFilters.erase(iter);
                return true;
            }
        }
        return false;
    }

    bool ImageHandler::deleteFilterByIndex(GLuint index, bool bDelMem) {
        if (index >= vecFilters.size())
            return false;
        if (bDelMem) {
            CGE_ENABLE_GLOBAL_GLCONTEXT();
            delete vecFilters[index];
        }
        vecFilters.erase(vecFilters.begin() + index);
        return true;
    }

    bool ImageHandler::replaceFilterAtIndex(ImageFilterInterfaceAbstract *proc, GLuint index,
                                            bool bDelMem) {
        if (index >= vecFilters.size())
            return false;
        std::vector<ImageFilterInterfaceAbstract *>::iterator iter =
                vecFilters.begin() + index;
        if (bDelMem) {
            CGE_ENABLE_GLOBAL_GLCONTEXT();
            delete *iter;
        }
        *iter = proc;
        return true;
    }

    bool ImageHandler::swapFilterByIndex(GLuint left, GLuint right) {
        if (left == right || left >= vecFilters.size() || right >= vecFilters.size())
            return false;
        std::swap(*(vecFilters.begin() + left), *(vecFilters.begin() + right));
        return true;
    }

}
