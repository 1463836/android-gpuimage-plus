/*
* cgeShaderFunctions.cpp
*
*  Created on: 2013-12-5
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "CommonDefine.h"
#include "GLFunctions.h"
#include "ShaderFunctions.h"

CGE_UNEXPECTED_ERR_MSG
(
        static int sProgramCount = 0;

)

namespace CGE {
    ShaderObject::ShaderObject() : shaderType(GL_FALSE), shaderID(0) {}

    ShaderObject::~ShaderObject() { clear(); }

    bool ShaderObject::init(GLenum _shaderType) {
        shaderType = _shaderType;
        if (shaderID == 0)
            shaderID = glCreateShader(shaderType);
        return shaderID != 0;
    }

    void ShaderObject::clear() {
        if (shaderID == 0) return;
        glDeleteShader(shaderID);
        shaderID = 0;
        shaderType = GL_FALSE;
    }

    bool ShaderObject::loadShaderSourceFromString(const char *shaderString) {
        if (shaderID == 0) {
            shaderID = glCreateShader(shaderType);
            CGE_LOG_CODE(
                    if (shaderID == 0) {
                        loge("glCreateShader Failed!");
                        return false;
                    })
        }
        glShaderSource(shaderID, 1, (const GLchar **) &shaderString, nullptr);
        glCompileShader(shaderID);
        GLint compiled = 0;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);

        if (compiled == GL_TRUE) return true;

        CGE_LOG_CODE(
                GLint
                        logLen;
                glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLen);
                if (logLen > 0) {
                    char *buf = new char[logLen];
                    if (buf != nullptr) {
                        glGetShaderInfoLog(shaderID, logLen, &logLen, buf);
                        loge("Shader %d compile faild: \n%s\n", shaderID, buf);
                        delete[] buf;
                    }
                })
        return false;
    }

    //////////////////////////////////////////////////////////////////////////

    UniformParameters::~UniformParameters() {
        clear();
    }

    void UniformParameters::pushi(const char *name, GLint x) {
        UniformData *data = new UniformData(name, uniformINT);
        data->setValuesi(x);
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::pushi(const char *name, GLint x, GLint y) {
        UniformData *data = new UniformData(name, uniformINTV2);
        data->setValuesi(x, y);
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::pushi(const char *name, GLint x, GLint y, GLint z) {
        UniformData *data = new UniformData(name, uniformINTV3);
        data->setValuesi(x, y, z);
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::pushi(const char *name, GLint x, GLint y, GLint z, GLint w) {
        UniformData *data = new UniformData(name, uniformINTV4);
        data->setValuesi(x, y, z, w);
        m_vecUniforms.push_back(data);
    }


    void UniformParameters::pushf(const char *name, GLfloat x) {
        UniformData *data = new UniformData(name, uniformFLOAT);
        data->setValuesf(x);
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::pushf(const char *name, GLfloat x, GLfloat y) {
        UniformData *data = new UniformData(name, uniformFLOATV2);
        data->setValuesf(x, y);
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::pushf(const char *name, GLfloat x, GLfloat y, GLfloat z) {
        UniformData *data = new UniformData(name, uniformFLOATV3);
        data->setValuesf(x, y, z);
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::pushf(const char *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
        UniformData *data = new UniformData(name, uniformFLOATV4);
        data->setValuesf(x, y, z, w);
        m_vecUniforms.push_back(data);
    }

    void
    UniformParameters::pushSampler1D(const char *name, GLuint *textureID, GLint textureBindID) {
        UniformData *data = new UniformData(name, uniformSAMPLER1D);
        data->uniformValue[0].valueuPtr = textureID;
        data->uniformValue[1].valuei = textureBindID;
        m_vecUniforms.push_back(data);
    }

    void
    UniformParameters::pushSampler2D(const char *name, GLuint *textureID, GLint textureBindID) {
        UniformData *data = new UniformData(name, uniformSAMPLER2D);
        data->uniformValue[0].valueuPtr = textureID;
        data->uniformValue[1].valuei = textureBindID;
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::requireStepsFactor(const char *name) {
        UniformData *data = new UniformData(name, uniformStepsFactor);
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::requireRatioAspect(const char *name, GLfloat texAspectRatio) {
        UniformData *data = new UniformData(name, uniformRatioAspect);
        data->uniformValue[0].valuef = texAspectRatio;
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::requireStepsRatio(const char *name, GLfloat texAspectRatio) {
        UniformData *data = new UniformData(name, uniformStepsRatio);
        data->uniformValue[0].valuef = texAspectRatio;
        m_vecUniforms.push_back(data);
    }

    void UniformParameters::clear() {
        for (std::vector<UniformData *>::iterator iter = m_vecUniforms.begin();
             iter != m_vecUniforms.end(); ++iter)
            delete *iter;
        m_vecUniforms.clear();
    }

    void UniformParameters::assignUniforms(ImageHandlerInterface *hander, GLuint program) {
        for (std::vector<UniformData *>::iterator iter = m_vecUniforms.begin();
             iter != m_vecUniforms.end(); ++iter) {
            GLint uniformID = glGetUniformLocation(program, (*iter)->uniformName);
            if (uniformID < 0) {
                loge("Uniform name %s does not exist!\n", (*iter)->uniformName);
                return;
            }
            switch ((*iter)->uniformType) {
                case uniformINT:
                    glUniform1i(uniformID, (*iter)->uniformValue[0].valuei);
                    break;
                case uniformINTV2:
                    glUniform2i(uniformID, (*iter)->uniformValue[0].valuei,
                                (*iter)->uniformValue[1].valuei);
                    break;
                case uniformINTV3:
                    glUniform3i(uniformID, (*iter)->uniformValue[0].valuei,
                                (*iter)->uniformValue[1].valuei, (*iter)->uniformValue[3].valuei);
                    break;
                case uniformINTV4:
                    glUniform4i(uniformID, (*iter)->uniformValue[0].valuei,
                                (*iter)->uniformValue[1].valuei, (*iter)->uniformValue[2].valuei,
                                (*iter)->uniformValue[3].valuei);
                    break;
                case uniformFLOAT:
                    glUniform1f(uniformID, (*iter)->uniformValue[0].valuef);
                    break;
                case uniformFLOATV2:
                    glUniform2f(uniformID, (*iter)->uniformValue[0].valuef,
                                (*iter)->uniformValue[1].valuef);
                    break;
                case uniformFLOATV3:
                    glUniform3f(uniformID, (*iter)->uniformValue[0].valuef,
                                (*iter)->uniformValue[1].valuef, (*iter)->uniformValue[2].valuef);
                    break;
                case uniformFLOATV4:
                    glUniform4f(uniformID, (*iter)->uniformValue[0].valuef,
                                (*iter)->uniformValue[1].valuef, (*iter)->uniformValue[2].valuef,
                                (*iter)->uniformValue[3].valuef);
                    break;
                case uniformSAMPLER1D: {
#ifdef GL_TEXTURE_1D
                    int texutreBindID = CGE_TEXTURE_START + (*iter)->uniformValue[1].valuei;
                    glActiveTexture(texutreBindID);
                    glBindTexture(GL_TEXTURE_1D, *(*iter)->uniformValue[0].valueuPtr);
                    glUniform1i(uniformID, texutreBindID - GL_TEXTURE0);
#endif
                }
                    break;
                case uniformSAMPLER2D: {
                    int texutreBindID = CGE_TEXTURE_START + (*iter)->uniformValue[1].valuei;
                    glActiveTexture(texutreBindID);
                    glBindTexture(GL_TEXTURE_2D, *(*iter)->uniformValue[0].valueuPtr);
                    glUniform1i(uniformID, texutreBindID - GL_TEXTURE0);
                }
                    break;
                case uniformStepsFactor: {
                    Sizei sz = hander->getOutputFBOSize();
                    glUniform2f(uniformID, 1.0f / sz.width, 1.0f / sz.height);
                }
                    break;
                case uniformRatioAspect: {
                    Sizei sz = hander->getOutputFBOSize();
                    GLfloat x, y, z, w;
                    GLfloat asSrc = (GLfloat) sz.width / sz.height;
                    GLfloat asTex = (*iter)->uniformValue[0].valuef;
                    if (asSrc > asTex) {
                        x = 1.0f;
                        y = asTex / asSrc;
                        z = 0.0f;
                        w = (1.0f - y) / 2.0f;
                    } else {
                        x = asSrc / asTex;
                        y = 1.0;
                        z = (1.0f - x) / 2.0f;
                        w = 0.0f;
                    }
                    glUniform4f(uniformID, x, y, z, w);
                }
                    break;
                case uniformStepsRatio: {
                    Sizei sz = hander->getOutputFBOSize();
                    glUniform1f(uniformID,
                                float(sz.width) / sz.height / (*iter)->uniformValue[0].valuef);
                }
                    break;
                default:
                    loge("UniformParameters::assignUniforms: Uniform Type Not Supported!");
                    break;
            }
        }
    }

    UniformParameters::UniformData *UniformParameters::getDataPointerByName(const char *name) {
        for (std::vector<UniformData *>::iterator iter = m_vecUniforms.begin();
             iter != m_vecUniforms.end(); ++iter) {
            if (strcmp((*iter)->uniformName, name) == 0) {
                return *iter;
            }
        }
        return nullptr;
    }

    ProgramObject::ProgramObject() {
        programID = glCreateProgram();

        CGE_UNEXPECTED_ERR_MSG
        (
                CGE_LOG_KEEP("ProgramObject create, total: %d\n", ++sProgramCount);
        )
    }

    ProgramObject::~ProgramObject() {
        CGE_UNEXPECTED_ERR_MSG
        (
                CGE_LOG_KEEP("ProgramObject release, remain: %d\n", --sProgramCount);
        )

        if (programID == 0)
            return;
        GLuint attachedShaders[32];
        int numAttachedShaders = 0;
        glGetAttachedShaders(programID, 32, &numAttachedShaders, attachedShaders);
        for (int i = 0; i < numAttachedShaders; ++i) {
            glDetachShader(programID, attachedShaders[i]);
        }
        glDeleteProgram(programID);
    }

    bool ProgramObject::initFragmentShaderSourceFromString(const char *fragShader) {
        return fragmentObj.init(GL_FRAGMENT_SHADER) &&
               fragmentObj.loadShaderSourceFromString(fragShader);
    }

    bool ProgramObject::initVertexShaderSourceFromString(const char *vertShader) {
        return vertObj.init(GL_VERTEX_SHADER) && vertObj.loadShaderSourceFromString(vertShader);
    }

    bool ProgramObject::initWithShaderStrings(const char *vsh, const char *fsh) {
        return initVertexShaderSourceFromString(vsh) && initFragmentShaderSourceFromString(fsh) &&
               link();
    }

    bool ProgramObject::linkWithShaderObject(ShaderObject &vertObj, ShaderObject &fragObj,
                                             bool shouldClear) {
        if (programID != 0) {
            GLuint attachedShaders[32] = {0};
            int numAttachedShaders = 0;
            glGetAttachedShaders(programID, 32, &numAttachedShaders, attachedShaders);
            for (int i = 0; i < numAttachedShaders; ++i) {
                glDetachShader(programID, attachedShaders[i]);
            }
            cgeCheckGLError("Detach Shaders in useProgram");
        } else {
            programID = glCreateProgram();
        }
        GLint programStatus;
        glAttachShader(programID, vertObj.getShaderID());
        glAttachShader(programID, fragObj.getShaderID());
        cgeCheckGLError("Attach Shaders in useProgram");
        glLinkProgram(programID);
        glGetProgramiv(programID, GL_LINK_STATUS, &programStatus);

        if (shouldClear) {
            vertObj.clear();
            fragmentObj.clear();
        }

        if (programStatus != GL_TRUE) {
            GLint logLen = 0;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen != 0) {
                char *buf = new char[logLen];
                if (buf != nullptr) {
                    glGetProgramInfoLog(programID, logLen, &logLen, buf);
                    loge("Failed to link the program!\n%s", buf);
                    delete[] buf;
                }
            }
            loge("LINK %d Failed\n", programID);
            return false;
        }
        cgeCheckGLError("Link Program");
        return true;
    }

}
