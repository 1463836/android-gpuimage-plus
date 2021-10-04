/*
 * cgeDynamicFilters.h
 *
 *  Created on: 2015-11-18
 *      Author: Wang Yang
 */

#ifndef _CGEDYNAMICFILTERS_H_
#define _CGEDYNAMICFILTERS_H_

#include "DynamicWaveFilter.h"
#include "MotionFlowFilter.h"

namespace CGE {
    CGEDynamicWaveFilter *createDynamicWaveFilter();

    CGEMotionFlowFilter *createMotionFlowFilter();
}

#endif