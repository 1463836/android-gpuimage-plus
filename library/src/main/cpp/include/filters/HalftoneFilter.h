/*
 * cgeHalftoneFilter.h
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 */

#ifndef _CGEHALFTONEFILTER_H_
#define _CGEHALFTONEFILTER_H_

#include "GLFunctions.h"

namespace CGE {
    class CGEHalftoneFilter : public ImageFilterInterface {
    public:

        bool init();

        //Range: >= 1.
        void setDotSize(float value);

        void setIntensity(float value);

        void render2Texture(CGE::ImageHandlerInterface *handler, GLuint srcTexture,
                            GLuint vertexBufferID);

    protected:
        static CGEConstString paramAspectRatio;
        static CGEConstString paramDotPercent;
        float m_dotSize;
    };


}

#endif