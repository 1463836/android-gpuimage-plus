//
//  CustomFilter_0.h
//  filterLib
//
//  Created by wangyang on 16/8/2.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#ifndef CustomFilter_0_h
#define CustomFilter_0_h

#include "GLFunctions.h"

//standard demo

class CustomFilter_0 : public CGE::ImageFilterInterface {
public:

    bool init();

    void render2Texture(CGE::ImageHandlerInterface *handler, GLuint srcTexture,
                        GLuint vertexBufferID);

protected:
    GLint mStepLoc;
};

#endif
