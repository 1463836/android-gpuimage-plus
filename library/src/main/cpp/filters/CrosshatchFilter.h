/*
 * cgeCrosshatchFilter.h
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#ifndef _CGE_CROSSHATCH_H_
#define _CGE_CROSSHATCH_H_

#include "GLFunctions.h"

namespace CGE {
    class CGECrosshatchFilter : public ImageFilterInterface {
    public:

        //Range: (0, 0.1], default: 0.03
        void setCrosshatchSpacing(float value);

        //Range: (0, 0.01], default: 0.003
        void setLineWidth(float value);

        bool init();

    protected:
        static ConstString paramCrosshatchSpacing;
        static ConstString paramLineWidth;
    };

}

#endif