/*
 * cgeAdvancedEffects.h
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#ifndef _CGEADVANCEDEFFECTS_H_
#define _CGEADVANCEDEFFECTS_H_

#include "EmbossFilter.h"
#include "EdgeFilter.h"
#include "RandomBlurFilter.h"
#include "BilateralBlurFilter.h"
#include "MosaicBlurFilter.h"
#include "LiquifyFilter.h"
#include "HalftoneFilter.h"
#include "PolarPixellateFilter.h"
#include "PolkaDotFilter.h"
#include "CrosshatchFilter.h"
#include "HazeFilter.h"
#include "LerpblurFilter.h"

#include "SketchFilter.h"
#include "BeautifyFilter.h"

namespace CGE {
    CGEEmbossFilter *createEmbossFilter();

    CGEEdgeFilter *createEdgeFilter();

    CGEEdgeSobelFilter *createEdgeSobelFilter();

    CGERandomBlurFilter *createRandomBlurFilter();

    CGEBilateralBlurFilter *createBilateralBlurFilter();

    CGEBilateralBlurBetterFilter *createBilateralBlurBetterFilter();

    CGEMosaicBlurFilter *createMosaicBlurFilter();

    LiquifyFilter *getLiquidationFilter(float ratio, float stride);

    LiquifyFilter *getLiquidationFilter(float width, float height, float stride);

    CGELiquidationNicerFilter *getLiquidationNicerFilter(float ratio, float stride);

    CGELiquidationNicerFilter *getLiquidationNicerFilter(float width, float height, float stride);

    CGEHalftoneFilter *createHalftoneFilter();

    CGEPolarPixellateFilter *createPolarPixellateFilter();

    CGEPolkaDotFilter *createPolkaDotFilter();

    CGECrosshatchFilter *createCrosshatchFilter();

    CGEHazeFilter *createHazeFilter();

    CGELerpblurFilter *createLerpblurFilter();

    CGESketchFilter *createSketchFilter();

    CGEBeautifyFilter *createBeautifyFilter();
}

#endif 
