/*﻿
* cgeImageFilter.h
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*/

#ifndef _CGEIMAGEFILTER_H_
#define _CGEIMAGEFILTER_H_

#include "GLFunctions.h"
#include "ShaderFunctions.h"

#ifndef CGE_CURVE_PRECISION
#define CGE_CURVE_PRECISION 256
#endif

namespace CGE {

    extern ConstString g_vshDefault;
    extern ConstString g_vshDefaultWithoutTexCoord;
    extern ConstString g_vshDrawToScreen;
    extern ConstString g_vshDrawToScreenRot90;
    extern ConstString g_fshDefault;
    extern ConstString g_fshFastAdjust;
    extern ConstString g_fshFastAdjustRGB;
    extern ConstString g_fshCurveMapNoIntensity;

    extern ConstString g_paramFastAdjustArrayName;
    extern ConstString g_paramFastAdjustRGBArrayName;
    extern ConstString g_paramCurveMapTextureName;


    class ImageHandlerInterface;

    class ImageFilterInterface;

    class ImageFilterInterfaceAbstract {
    public:
        ImageFilterInterfaceAbstract();

        virtual ~ImageFilterInterfaceAbstract();

        virtual void render2Texture(ImageHandlerInterface *handler, GLuint srcTexture,
                                    GLuint vertexBufferID) = 0;

        virtual void setIntensity(float value) {}

        //mutiple effects专有， 若返回为 true， handler在添加filter时会进行拆解。
        virtual bool isWrapper() { return false; }

        virtual std::vector<ImageFilterInterface *>
        getFilters(bool bMove = true) { return std::vector<ImageFilterInterface *>(); }

    };

    class ImageFilterInterface : public ImageFilterInterfaceAbstract {
    public:
        ImageFilterInterface();

        virtual ~ImageFilterInterface();

        virtual void
        render2Texture(ImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

        //////////////////////////////////////////////////////////////////////////
        bool initShadersFromString(const char *vsh, const char *fsh);
        //bool initShadersFromFile(const char* vshFileName, const char* fshFileName);

        void setAdditionalUniformParameter(UniformParameters *param);

        UniformParameters *getUniformParam() { return m_uniformParam; }

        virtual bool init() { return false; }

        ProgramObject &getProgram() { return m_program; }

        static ConstString paramInputImageName;
        static ConstString paramPositionIndexName;

        //protected:
        //////////////////////////////////////////////////////////////////////////
        //virtual bool initVertexShaderFromString(const char* vsh);
        //	virtual bool initVertexShaderFromFile(const char* vshFileName);

        //virtual bool initFragmentShaderFromString(const char* fsh);
        //	virtual bool initFragmentShaderFromFile(const char* fshFileName);

        //virtual bool finishLoadShaders(); //如果单独调用上方函数初始化，请在结束后调用本函数。

    protected:
        ProgramObject m_program;

        //See the description of "UniformParameters" to know "How to use it".
        UniformParameters *m_uniformParam;
    };

    class CGEFastAdjustFilter : public ImageFilterInterface {
    public:

        typedef struct CurveData {
            float data[3];

            float &operator[](int index) {
                return data[index];
            }

            const float &operator[](int index) const {
                return data[index];
            }
        } CurveData;

        bool init();

    protected:
        static ConstString paramArray;

        void assignCurveArrays();

        void initCurveArrays();

    protected:
        std::vector<CurveData> m_curve;
    };

    class CGEFastAdjustRGBFilter : public ImageFilterInterface {
    public:

        bool init();

    protected:
        static ConstString paramArrayRGB;

        void assignCurveArray();

        void initCurveArray();

    protected:
        std::vector<float> m_curveRGB;
    };

}

#endif
