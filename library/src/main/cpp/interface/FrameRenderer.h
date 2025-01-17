/*
* cgeFrameRenderer.h
*
*  Created on: 2015-11-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGEFRAMERENDERER_H_
#define _CGEFRAMERENDERER_H_

#include "ImageHandler.h"
#include "Thread.h"
#include "SharedGLContext.h"

#include "TextureUtils.h"
#include "Vec.h"


namespace CGE {
    class FastFrameHandler : public ImageHandler {
    public:

        void processingFilters();

        void swapBufferFBO();

        inline void useImageFBO() {
            ImageHandler::useImageFBO();
        }
    };


    class FrameRenderer {
    public:
        FrameRenderer();

        virtual ~FrameRenderer();

        //srcWidth, srcHeight为external texture的使用宽高(旋转之后的值)
        //dstWidth, dstHeight为保存video使用的宽高， 默认640*480
        bool init(int srcWidth, int srcHeight, int dstWidth = 640, int dstHeight = 480);

        //srcTexture 尺寸改变, 将重新计算viewport等
        void srcResize(int width, int height);

        //会影响framebuffer设置。
        virtual void update(GLuint textureOES, float *matrix);

        //滤镜process接口, 不调用将不产生滤镜效果
        virtual void runProc();

        //不设置framebuffer， 若要绘制到屏幕， 需绑定到0
        void render(int x, int y, int width, int height);

        //绘制一定不带mask的结果
        //注意, drawCache和render、update 三个函数在使用时非线程安全。
        void drawCache();

        //不设置framebuffer, viewport等， 直接绘制
        // void renderResult();
        // void renderResultWithMask();

        //设置external texture的旋转值
        void setSrcRotation(float rad);

        //设置external texture的翻转缩放值
        void setSrcFlipScale(float x, float y);

        //设置渲染结果旋转(弧度)
        void setRenderRotation(float rad);

        //设置渲染结果翻转缩放
        void setRenderFlipScale(float x, float y);

        void
        setFilterWithConfig(ConstString config, CGETextureLoadFun cgeGlobalTextureLoadFunc, void *cgeTexLoadArg);

        void setFilterIntensity(float value);

        void setFilter(ImageFilterInterfaceAbstract *filter);

        GLuint getTargetTexture();

        GLuint getBufferTexture();

        void setMaskTexture(GLuint maskTexture, float aspectRatio);

        void setMaskTextureRatio(float aspectRatio);

        void setMaskRotation(float rad);

        void setMaskFlipScale(float x, float y);

        inline FastFrameHandler *getImageHandler() { return fastFrameHandler; }

        inline void bindImageFBO() { fastFrameHandler->useImageFBO(); }

        inline void swapBufferFBO() { fastFrameHandler->swapBufferFBO(); }

    protected:
        void calcViewport(int srcWidth, int srcHeight, int dstWidth, int dstHeight);

    protected:

        FastFrameHandler *fastFrameHandler;
        //主要用于重绘external_OES类型texture
        TextureDrawer *textureDrawBase;
        TextureDrawer4ExtOES *textureDrawOES;

        Sizei srcSize, dstSize;
        Vec4i viewport;

        TextureDrawer *textureDrawCache;
        bool m_isUsingMask;

        //辅助参数
        float m_drawerFlipScaleX, m_drawerFlipScaleY;

        std::mutex resultMutex;
    };

}


#endif