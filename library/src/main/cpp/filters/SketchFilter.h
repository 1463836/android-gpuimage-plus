/*
* cgeSketchFilter.h
*
*  Created on: 2015-3-20
*      Author: Wang Yang
*/

#ifndef _CGE_SKETCHFILTER_H_
#define _CGE_SKETCHFILTER_H_

#include "MaxValueFilter.h"
#include "MultipleEffects.h"

namespace CGE {
    class CGESketchFilter : public ImageFilterInterface {
    public:

        CGESketchFilter();

        ~CGESketchFilter();

        bool init();

        void setIntensity(float intensity);

        void render2Texture(CGE::ImageHandlerInterface *handler, GLuint srcTexture,
                            GLuint vertexBufferID);

        void flush();

    protected:

        static ConstString paramCacheTextureName;
        static ConstString paramIntensityName;

        CGEMaxValueFilter3x3 m_maxValueFilter;
        GLuint m_textureCache;
        Sizei m_cacheSize;
    };
}

#endif