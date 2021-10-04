/*
* cgeBeautifyFilter.h
*
*  Created on: 2016-3-22
*      Author: Wang Yang
*/

#ifndef _CGE_BEAUTIFYFILTER_H_
#define _CGE_BEAUTIFYFILTER_H_

#include "ImageFilter.h"

namespace CGE {
    class CGEBeautifyFilter : public ImageFilterInterface {
    public:

        bool init();

        void setIntensity(float intensity);

        void setImageSize(float width, float height, float mul = 1.5f);

        void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

    protected:
        float m_intensity;
    };
}

#endif
