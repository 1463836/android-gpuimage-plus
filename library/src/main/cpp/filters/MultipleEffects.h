﻿/*
 * cgeMultipleEffects.h
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 *        Blog: http://wysaid.org
*/

#ifndef _CGEMUTIPLEEFFECTS_H_
#define _CGEMUTIPLEEFFECTS_H_

#include "GLFunctions.h"

namespace CGE {
    //It's just a help class for CGEMutipleEffectFilter.
    class CGEMutipleMixFilter : protected ImageFilterInterface {
    public:
        CGEMutipleMixFilter() {}

        void setIntensity(float value);

        bool init();

        void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

        bool needToMix();

        bool noIntensity();

    protected:
        static ConstString paramIntensityName;
        static ConstString paramOriginImageName;

    private:
        float m_intensity;
    };


    //This class is inherited from top filter interface and would do more processing steps
    // than "CGEImageFilter" does.
    class CGEMutipleEffectFilter : public ImageFilterInterfaceAbstract {
    public:
        CGEMutipleEffectFilter();

        ~CGEMutipleEffectFilter();

        void
        setBufferLoadFunction(CGEBufferLoadFun fLoad, void *loadParam, CGEBufferUnloadFun fUnload,
                              void *unloadParam);

        void setTextureLoadFunction(CGETextureLoadFun texLoader, void *arg);

        // bool initWithEffectID(int index);
        bool initWithEffectString(const char *pstr);

        bool initCustomize(); //特殊用法， 自由组合

        void setIntensity(float value); //设置混合程度
        bool isEmpty() { return m_vecFilters.empty(); }

        void clearFilters();

        bool isWrapper() { return m_isWrapper; }

        std::vector<ImageFilterInterface *> getFilters(bool bMove = true);

        std::vector<ImageFilterInterface *> &vecFilters() {
            return m_vecFilters;
        }

        //////////////////////////////////////////////////////////////////////////

        void
        render2Texture(ImageHandlerInterface *fastFrameHandler, GLuint srcTexture, GLuint vertexBufferID);

        void addFilter(ImageFilterInterface *proc) {
            if (proc != nullptr)
                m_vecFilters.push_back(proc);
        }

        GLuint loadResources(const char *textureName, int *w = nullptr, int *h = nullptr);

        CGEBufferLoadFun getLoadFunc() { return m_loadFunc; };

        CGEBufferUnloadFun getUnloadFunc() { return m_unloadFunc; }

        CGETextureLoadFun getTexLoadFunc() { return m_texLoadFunc; }

        void *getLoadParam() { return m_loadParam; }

        void *getUnloadParam() { return m_unloadParam; }

        void *getTexLoadParam() { return m_texLoadParam; }

    protected:
        CGEBufferLoadFun m_loadFunc;
        CGEBufferUnloadFun m_unloadFunc;
        CGETextureLoadFun m_texLoadFunc;
        void *m_loadParam;
        void *m_unloadParam;
        void *m_texLoadParam;
        std::vector<ImageFilterInterface *> m_vecFilters;
        CGEMutipleMixFilter m_mixFilter;

        Sizei m_currentSize;
        GLuint m_texCache;
        bool m_isWrapper;
    };
}


#endif /* _CGEMUTIPLEEFFECTS_H_ */
