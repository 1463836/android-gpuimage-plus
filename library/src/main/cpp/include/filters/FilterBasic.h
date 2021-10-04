/*
 * cgeFilterBasic.h
 *
 *  Created on: 2013-12-25
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifndef _CGEBASICADJUST_H_
#define _CGEBASICADJUST_H_

#include "BrightnessAdjust.h"
#include "ContrastAdjust.h"
#include "SharpenBlurAdjust.h"
#include "SaturationAdjust.h"
#include "ShadowHighlightAdjust.h"
#include "WhiteBalanceAdjust.h"
#include "MonochromeAdjust.h"
#include "CurveAdjust.h"
#include "ColorLevelAdjust.h"
#include "VignetteAdjust.h"
#include "TiltshiftAdjust.h"
#include "SharpenBlurAdjust.h"
#include "SelectiveColorAdjust.h"
#include "ExposureAdjust.h"
#include "HueAdjust.h"
#include "ColorBalanceAdjust.h"
#include "LookupFilter.h"

namespace CGE {
    CGEBrightnessFilter *createBrightnessFilter();

    CGEBrightnessFastFilter *createBrightnessFastFilter();

    CGEContrastFilter *createContrastFilter();

    CGESharpenBlurFilter *createSharpenBlurFilter();

    CGESharpenBlurFastFilter *createSharpenBlurFastFilter();

    CGESharpenBlurSimpleFilter *createSharpenBlurSimpleFilter();

    CGESharpenBlurSimpleBetterFilter *createSharpenBlurSimpleBetterFilter();

    CGESaturationHSLFilter *createSaturationHSLFilter();

    CGESaturationFilter *createSaturationFilter();

    CGEShadowHighlightFilter *createShadowHighlightFilter();

    CGEShadowHighlightFastFilter *createShadowHighlightFastFilter();

    CGEWhiteBalanceFilter *createWhiteBalanceFilter();

    CGEWhiteBalanceFastFilter *createWhiteBalanceFastFilter();

    CGEMonochromeFilter *createMonochromeFilter(); // 黑白
    CGECurveTexFilter *createCurveTexFilter();

    CGECurveFilter *createCurveFilter();

    CGEMoreCurveFilter *createMoreCurveFilter();

    CGEMoreCurveTexFilter *createMoreCurveTexFilter();

    CGEColorLevelFilter *createColorLevelFilter();

    CGEVignetteFilter *createVignetteFilter();

    CGEVignetteExtFilter *createVignetteExtFilter();

    CGETiltshiftVectorFilter *createTiltshiftVectorFilter();

    CGETiltshiftEllipseFilter *createTiltshiftEllipseFilter();

    CGETiltshiftVectorWithFixedBlurRadiusFilter *createFixedTiltshiftVectorFilter();

    CGETiltshiftEllipseWithFixedBlurRadiusFilter *createFixedTiltshiftEllipseFilter();

    CGESharpenBlurFastWithFixedBlurRadiusFilter *createSharpenBlurFastWithFixedBlurRadiusFilter();

    CGESelectiveColorFilter *createSelectiveColorFilter();

    CGEExposureFilter *createExposureFilter();

    CGEHueAdjustFilter *createHueAdjustFilter();

    CGEColorBalanceFilter *createColorBalanceFilter();

    CGELookupFilter *createLookupFilter();
}

#endif 
