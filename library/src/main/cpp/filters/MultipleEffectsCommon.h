/*
 * cgeMultipleEffectsCommon.h
 *
 *  Created on: 2014-1-2
 *      Author: Wang Yang
 */

#ifndef _CGEMUTIPLEEFFECTSCOMMON_H_
#define _CGEMUTIPLEEFFECTSCOMMON_H_

#include "GLFunctions.h"
#include "CurveAdjust.h"

namespace CGE {
    void cgeEnableColorScale();

    void cgeDisableColorScale();

    //////////////////////////////////////////////////////////////////////////

    class CGELomoFilter : public ImageFilterInterface {
    public:
        CGELomoFilter() : m_scaleDark(-1.0f), m_scaleLight(-1.0f), m_saturate(1.0f) {}

        bool init();

        void setVignette(float start, float end);

        void setIntensity(float value);

        void setSaturation(float value);

        void setColorScale(float low, float range);

        void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);


    protected:
        static ConstString paramColorScaleName;
        static ConstString paramSaturationName;
        static ConstString paramVignetteName;
        static ConstString paramAspectRatio;
        static ConstString paramIntensityName;

    private:
        GLfloat m_scaleDark, m_scaleLight, m_saturate;
    };

    class CGELomoLinearFilter : public CGELomoFilter {
    public:
        bool init();
    };

    //////////////////////////////////////////////////////////////////////////

    class CGELomoWithCurveFilter : public CGEMoreCurveFilter {
    public:
        CGELomoWithCurveFilter() : m_scaleDark(-1.0f), m_scaleLight(-1.0f), m_saturate(1.0f) {}

        virtual bool init();

        void setVignette(float start, float end);

        void setSaturation(float value);

        void setColorScale(float low, float range);

        void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

    protected:
        static ConstString paramColorScaleName;
        static ConstString paramSaturationName;
        static ConstString paramVignetteName;
        static ConstString paramAspectRatio;

    private:
        GLfloat m_scaleDark, m_scaleLight, m_saturate;
    };

    class CGELomoWithCurveLinearFilter : public CGELomoWithCurveFilter {
    public:
        bool init();
    };

    //////////////////////////////////////////////////////////////////////////

    class CGELomoWithCurveTexFilter : public CGELomoWithCurveFilter {
    public:
        virtual bool init();

        virtual void flush();

    protected:
        void initSampler();

    protected:
        GLuint m_curveTexture;
    };

    class CGELomoWithCurveTexLinearFilter : public CGELomoWithCurveTexFilter {
    public:
        bool init();
    };

    //////////////////////////////////////////////////////////////////////////

    //TODO: 重写colorscale filter， 避免CPU方法!
    class CGEColorScaleFilter : public ImageFilterInterface {
    public:
        CGEColorScaleFilter() : m_scaleDark(-1.0f), m_scaleLight(-1.0f), m_saturate(1.0f) {}

        ~CGEColorScaleFilter() {}

        virtual bool init();

        void setColorScale(float low, float range);

        //Set saturation value to -1.0 ( < 0.0 ) when your shader program did nothing with this value.
        void setSaturation(float value);

        virtual void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

    protected:
        static ConstString paramColorScaleName;
        static ConstString paramSaturationName;
    private:
        GLfloat m_scaleDark, m_scaleLight, m_saturate;
    };

    class CGEColorMulFilter : public ImageFilterInterface {
    public:

        enum MulMode {
            mulFLT, mulVEC, mulMAT
        };

        bool initWithMode(MulMode mode);

        void setFLT(float value);

        void setVEC(float r, float g, float b);

        void setMAT(float *mat); //The lenth of "mat" must be at least 9.
    protected:
        static ConstString paramMulName;
    };

}


#endif
