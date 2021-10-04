/*
 * cgeAdvancedEffectsCommon.h
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#ifndef _CGEADVANCEDEFFECTSCOMMON_H_
#define _CGEADVANCEDEFFECTSCOMMON_H_

#include "GLFunctions.h"
#include "ImageFilter.h"
#include "ImageHandler.h"

namespace CGE {
    class CGEAdvancedEffectOneStepFilterHelper : public ImageFilterInterface {
    public:
        CGEAdvancedEffectOneStepFilterHelper() {}

        ~CGEAdvancedEffectOneStepFilterHelper() {}

        virtual void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

    protected:
        static CGEConstString paramStepsName;
    };

    class CGEAdvancedEffectTwoStepFilterHelper : public ImageFilterInterface {
    public:
        CGEAdvancedEffectTwoStepFilterHelper() {}

        ~CGEAdvancedEffectTwoStepFilterHelper() {}

        virtual void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

    protected:
        static CGEConstString paramStepsName;
    };

}

#endif
