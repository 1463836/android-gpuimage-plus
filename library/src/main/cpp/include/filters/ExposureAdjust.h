/*
 * cgeExposureAdjust.h
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 */

#ifndef _CGEEXPOSUREADJUST_H_
#define _CGEEXPOSUREADJUST_H_

#include "GLFunctions.h"

namespace CGE {
    class CGEExposureFilter : public ImageFilterInterface {
    public:

        //Range: [-10, 10]
        void setIntensity(float value);

        bool init();

    protected:
        static CGEConstString paramName;

    };
}

#endif