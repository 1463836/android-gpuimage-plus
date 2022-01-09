/*
 * cgeHazeFilter.cpp
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#include "HazeFilter.h"

static ConstString s_fshHaze = CGE_SHADER_STRING_PRECISION_M
                                  (
                                          varying vec2 textureCoordinate;

                                          uniform sampler2D inputImageTexture;

                                          uniform float dis;
                                          uniform float slope;
                                          uniform vec3 hazeColor;

                                          void main() {
                                              float d = textureCoordinate.y * slope + dis;

                                              vec4 c = texture2D(inputImageTexture,
                                                                 textureCoordinate);

                                              c.rgb = (c.rgb - d * hazeColor.rgb) / (1.0 - d);

                                              gl_FragColor = c;
                                          });

namespace CGE {
    ConstString CGEHazeFilter::paramDistance = "dis";
    ConstString CGEHazeFilter::paramSlope = "slope";
    ConstString CGEHazeFilter::paramHazeColor = "hazeColor";

    bool CGEHazeFilter::init() {
        if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshHaze)) {
            setDistance(0.2f);
            setHazeColor(1.0f, 1.0f, 1.0f);
            return true;
        }
        return false;
    }

    void CGEHazeFilter::setDistance(float value) {
        m_program.bind();
        m_program.sendUniformf(paramDistance, value);
    }

    void CGEHazeFilter::setSlope(float value) {
        m_program.bind();
        m_program.sendUniformf(paramSlope, value);
    }

    void CGEHazeFilter::setHazeColor(float r, float g, float b) {
        m_program.bind();
        m_program.sendUniformf(paramHazeColor, r, g, b);
    }
}