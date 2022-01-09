/*
 * cgeSaturationAdjust.h
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 */

#ifndef _CGESATURATION_ADJUST_H_
#define _CGESATURATION_ADJUST_H_

#include "GLFunctions.h"
#include "ImageFilter.h"
#include "ImageHandler.h"
#include "CurveAdjust.h"

namespace CGE {
    //This one is based on HSL.
    class CGESaturationHSLFilter : public ImageFilterInterface {
    public:
        CGESaturationHSLFilter() {}

        ~CGESaturationHSLFilter() {}

        void setSaturation(float value); // range [-1, 1]

        void setHue(float value); // range [-1, 1]

        void setLum(float lum); // range [-1, 1]

        bool init();

    protected:
        static ConstString paramSaturationName;
        static ConstString paramHueName;
        static ConstString paramLuminanceName;
    };

    // You can use the fast one instead(of the one above).
    class CGESaturationFilter : public ImageFilterInterface {
    public:
        CGESaturationFilter() {}

        ~CGESaturationFilter() {}

        bool init();

        //range: >0, 1 for origin, and saturation would increase if value > 1
        void setIntensity(float value);

    protected:
        static ConstString paramIntensityName;
    };

    //This one is based on HSV
    class CGESaturationHSVFilter : public ImageFilterInterface {
    public:
        CGESaturationHSVFilter() {}

        ~CGESaturationHSVFilter() {}

        bool init();

        //range: [-1, 1]
        void setAdjustColors(float red, float green, float blue,
                             float magenta, float yellow, float cyan);

    protected:
        static ConstString paramColor1;
        static ConstString paramColor2;
    };
}

#endif
