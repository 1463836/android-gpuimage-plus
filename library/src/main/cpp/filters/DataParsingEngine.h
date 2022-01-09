/*
* cgeMultipleEffects.h
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*/

#ifndef _CGEDATAPARSINGENGINE_H_
#define _CGEDATAPARSINGENGINE_H_

#include "MultipleEffects.h"

namespace CGE {
    class CGEDataParsingEngine {
    public:
        static ImageFilterInterface *
        adjustParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        curveParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        lomoWithCurveParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        lomoParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        blendParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        vignetteBlendParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        colorScaleParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        pixblendParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        krblendParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        vignetteParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        selfblendParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        colorMulParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        selectiveColorParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        blendTileParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        advancedStyleParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        beautifyParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        blurParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

        static ImageFilterInterface *
        dynamicParser(const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);

    };

}
#endif /* _CGEDATAPARSINGENGINE_H_ */
