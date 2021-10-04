//
//  CustomHelper.h
//  filterLib
//
//  Created by wysaid on 16/1/11.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#ifndef CommonHelper_h
#define CommonHelper_h

#include "GLFunctions.h"
#include "CustomFilters.h"

namespace CGE {
    ImageFilterInterface *cgeCreateCustomFilterByType(CustomFilterType type);
}

#endif
