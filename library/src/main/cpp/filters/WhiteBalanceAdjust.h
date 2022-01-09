/*
* cgeWhiteBalanceAdjust.h
*
*  Created on: 2013-12-26
*      Author: Wang Yang
*/

#ifndef _CGEWHITEBALANCE_H_
#define _CGEWHITEBALANCE_H_

#include "GLFunctions.h"
#include "ImageFilter.h"
#include "ImageHandler.h"

namespace CGE {
    class CGEWhiteBalanceFilter : public ImageFilterInterface {
    public:
        CGEWhiteBalanceFilter() {}

        ~CGEWhiteBalanceFilter() {}

        void setTemperature(float value); //range: -1~1, 0 for origin
        void setTint(float value);// range 0~5, 1 for origin

        bool init();

    protected:
        static ConstString paramTemperatureName;
        static ConstString paramTintName;
    };

    class CGEWhiteBalanceFastFilter : public ImageFilterInterface {
    public:
        CGEWhiteBalanceFastFilter() : m_temp(0.0f), m_tint(1.0f) {}

        ~CGEWhiteBalanceFastFilter() {}

        void setTempAndTint(float temp, float tint);

        bool init();

        float getTemp() { return m_temp; }

        float getTint() { return m_tint; }

    protected:
        static ConstString paramBalanceName;

    private:
        float m_temp, m_tint;
    };

}

#endif