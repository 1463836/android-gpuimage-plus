/*
 * cgeContrastAdjust.h
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 */

#ifndef _CGECONTRAST_ADJUST_H_
#define _CGECONTRAST_ADJUST_H_

#include "GLFunctions.h"
#include "ImageFilter.h"
#include "ImageHandler.h"

namespace CGE {
    class CGEContrastFilter : public ImageFilterInterface {
    public:
        CGEContrastFilter() {}

        ~CGEContrastFilter() {}

        void setIntensity(float value); //range > 0, and 1 for origin

        bool init();

    protected:
        static ConstString paramName;
    };
}

#endif
