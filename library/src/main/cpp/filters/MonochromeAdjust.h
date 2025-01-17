﻿/*
 * cgeMonochromeAdjust.h
 *
 *  Created on: 2013-12-29
 *      Author: Wang Yang
 */

#ifndef _CGEMONOCHROME_ADJUST_H_
#define _CGEMONOCHROME_ADJUST_H_

#include "GLFunctions.h"
#include "ImageFilter.h"
#include "ImageHandler.h"

namespace CGE {
    class CGEMonochromeFilter : public ImageFilterInterface {
    public:
        CGEMonochromeFilter() {}

        ~CGEMonochromeFilter() {}

        bool init();

        void setRed(float value);

        void setGreen(float value);

        void setBlue(float value);

        void setCyan(float value);

        void setMagenta(float value);

        void setYellow(float value);

    protected:
        static ConstString paramRed;
        static ConstString paramGreen;
        static ConstString paramBlue;
        static ConstString paramCyan;
        static ConstString paramMagenta;
        static ConstString paramYellow;
    };
}

#endif 
