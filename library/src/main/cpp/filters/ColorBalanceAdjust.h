/*
 * cgeColorBalanceAdjust.h
 *
 *  Created on: 2015-3-30
 *      Author: Wang Yang
 */

#ifndef _CGECOLORBALANCEADJUST_H_
#define _CGECOLORBALANCEADJUST_H_

#include "GLFunctions.h"

namespace CGE {
    class CGEColorBalanceFilter : public ImageFilterInterface {
    public:

        bool init();

        //Range[-1, 1], cyan to red
        void setRedShift(float value);

        //Range[-1, 1], magenta to green
        void setGreenShift(float value);

        //Range[-1, 1], yellow to blue
        void setBlueShift(float value);

    protected:
        static ConstString paramRedShiftName;
        static ConstString paramGreenShiftName;
        static ConstString paramBlueShiftName;

    };


}

#endif