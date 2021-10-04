/*
* cgeMinValueFilter.h
*
*  Created on: 2015-3-20
*      Author: Wang Yang
* Description: 最小值滤波
*/

#ifndef _CGE_MINVALUE_FILTER_H_
#define _CGE_MINVALUE_FILTER_H_

#include "GLFunctions.h"

namespace CGE {
    class CGEMinValueFilter3x3 : public ImageFilterInterface {
    public:

        bool init();

        void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

        GLint getStepsLocation() { return m_samplerStepsLoc; }

    protected:

        static CGEConstString paramSamplerStepsName;

        virtual const char *getShaderCompFunc();

        void initLocations();

    private:
        GLint m_samplerStepsLoc;
    };

    class CGEMinValueFilter3x3Plus : public CGEMinValueFilter3x3 {
    public:

        bool init();
    };

}

#endif