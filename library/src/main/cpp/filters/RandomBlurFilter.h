﻿/*
 * cgeRandomBlurFilter.h
 *
 *  Created on: 2013-12-29
 *      Author: Wang Yang
 */

#ifndef _CGERANDOMBLUR_H_
#define _CGERANDOMBLUR_H_

#include "AdvancedEffectsCommon.h"

namespace CGE {
    class CGERandomBlurFilter : public CGEAdvancedEffectOneStepFilterHelper {
    public:
        CGERandomBlurFilter() {}

        ~CGERandomBlurFilter() {}

        void setIntensity(float value);

        void setSamplerScale(float value);

        bool init();

    protected:
        static ConstString paramIntensity;
        static ConstString paramSamplerScale;
        static ConstString paramSamplerRadius;
    };
}

#endif 