//
// Created by Yunzhe on 2017/12/5.
//

#ifndef GPUBASEDRENDERING_A1_SHADER_H
#define GPUBASEDRENDERING_A1_SHADER_H

#include "global.h"
#include "textfile.h"
#include <map>

// Using namespace std for cout
using namespace std;

class GLSLProgramException : public runtime_error {
public:
    GLSLProgramException(const string &msg) :
            runtime_error(msg) {}
};

namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        TESS_CONTROL = GL_TESS_CONTROL_SHADER,
        TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
        COMPUTE = GL_COMPUTE_SHADER
    };
};

class Shader {
private:
    GLuint shaderProgram;
    bool linked;
    map<string, int> uniformLocations;

    GLint getUniformLocation(const char *name);

    bool fileExists(const string &fileName);

    string getExtension(const char *fileName);
public:
    Shader();

    virtual ~Shader();

    void compileShader(const char *fileName) throw(GLSLProgramException);

    void compileShader(const char *fileName, GLSLShader::GLSLShaderType type) throw(GLSLProgramException);

    void compileShader(const string &source, GLSLShader::GLSLShaderType type,
                       const char *fileName = nullptr) throw(GLSLProgramException);

    void link() throw(GLSLProgramException);

    void use() throw(GLSLProgramException);

    GLuint getProgram();

    void disable();

    void bindAttribLocation(GLuint location, const char *name);

    void bindFragDataLocation(GLuint location, const char *name);

    void setUniform(const char *name, float x, float y, float z);

    void setUniform(const char *name, const vec2 &v);

    void setUniform(const char *name, const vec3 &v);

    void setUniform(const char *name, const vec4 &v);

    void setUniform(const char *name, const mat4 &m);

    void setUniform(const char *name, const mat3 &m);

    void setUniform(const char *name, float val);

    void setUniform(const char *name, int val);

    void setUniform(const char *name, bool val);

    void setUniform(const char *name, GLuint val);

    void printActiveUniforms();

    void printActiveUniformBlocks();

    void printActiveAttribs();

    const char *getTypeString(GLenum type);
};

#endif //GPUBASEDRENDERING_A1_SHADER_H
