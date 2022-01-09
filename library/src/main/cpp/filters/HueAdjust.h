/*
 * cgeHueAdjust.h
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 */

#ifndef _CGEHUEADJUST_H_
#define _CGEHUEADJUST_H_

#include "GLFunctions.h"

namespace CGE {
    class CGEHueAdjustFilter : public ImageFilterInterface {
    public:

        //Range: [0, 2π]
        void setHue(float value);

        void setIntensity(float intensity);

        bool init();

    protected:
        static ConstString paramName;
    };

}

#endif