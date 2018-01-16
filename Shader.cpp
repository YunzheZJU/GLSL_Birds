//
// Created by Yunzhe on 2017/12/5.
//

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace GLSLShaderInfo {
    struct shader_file_extension {
        const char *ext;
        GLSLShader::GLSLShaderType type;
    };

    struct shader_file_extension extensions[] =
            {
                    {".vs",   GLSLShader::VERTEX},
                    {".vert", GLSLShader::VERTEX},
                    {".gs",   GLSLShader::GEOMETRY},
                    {".geom", GLSLShader::GEOMETRY},
                    {".tcs",  GLSLShader::TESS_CONTROL},
                    {".tes",  GLSLShader::TESS_EVALUATION},
                    {".fs",   GLSLShader::FRAGMENT},
                    {".frag", GLSLShader::FRAGMENT},
                    {".cs",   GLSLShader::COMPUTE}
            };
}

//
//void Shader::init() {
//    cout << "Creating program..." << endl;
//    shaderProgram = glCreateProgram();
//    if (shaderProgram == 0) {
//        cerr << "Error occurred when creating shader program." << endl;
//        exit(1);
//    } else {
//        cout << "Creating program...OK" << endl;
//    }
//}
//
//void Shader::set(const char *vertexShaderFile, const char *fragmentShaderFile) {
//    char *vertexShaderString = nullptr;
//    char *fragmentShaderString = nullptr;
//
//    glDetachShader(shaderProgram, vertexShader);
//    glDetachShader(shaderProgram, vertexShader);
//
//    cout << "Creating shaders..." << endl;
//    vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    if (vertexShader == 0 || fragmentShader == 0) {
//        cerr << "Error occurred when creating shaders." << endl;
//        exit(1);
//    } else {
//        cout << "Creating shaders...OK" << endl;
//    }
//
//    cout << "Reading shader source files..." << endl;
//    vertexShaderString = textFileRead(vertexShaderFile);
//    fragmentShaderString = textFileRead(fragmentShaderFile);
//    if (vertexShaderString == nullptr || fragmentShaderString == nullptr) {
//        cerr << "Error occurred when reading shader source files." << endl;
//        exit(1);
//    } else {
//        cout << "Reading shader source files...OK" << endl;
//    }
//
//    const char *vv = vertexShaderString;
//    const char *ff = fragmentShaderString;
//
//    cout << "Sourcing..." << endl;
//    glShaderSource(vertexShader, 1, &vv, nullptr);
//    glShaderSource(fragmentShader, 1, &ff, nullptr);
//    cout << "Sourcing...OK" << endl;
//
//    free(vertexShaderString);
//    free(fragmentShaderString);
//
//    cout << "Compiling shaders..." << endl;
//    glCompileShader(vertexShader);
//    glCompileShader(fragmentShader);
//    GLint vertexShaderCompileResult;
//    GLint fragmentShaderCompileResult;
//    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompileResult);
//    if (vertexShaderCompileResult == GL_FALSE) {
//        cerr << "Fail to compile vertex shader." << endl;
//        logShader(vertexShader);
//        exit(1);
//    } else {
//        cout << "Compiling vertex shader...OK" << endl;
//    }
//    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompileResult);
//    if (fragmentShaderCompileResult == GL_FALSE) {
//        cerr << "Fail to compile fragment shader." << endl;
//        logShader(fragmentShader);
//        exit(1);
//    } else {
//        cout << "Compiling fragment shader...OK" << endl;
//    }
//
//    cout << "Attaching..." << endl;
//    glAttachShader(shaderProgram, fragmentShader);
//    glAttachShader(shaderProgram, vertexShader);
//    cout << "Attaching...OK" << endl;
//
//    cout << "Linking..." << endl;
//    glLinkProgram(shaderProgram);
//    GLint linkStatus;
//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
//    if (linkStatus == GL_FALSE) {
//        cerr << "Fail to link shader program." << endl;
//        logProgram();
//        exit(1);
//    } else {
//        cout << "Linking...OK" << endl;
//    }
//};
//
//void Shader::enable() {
//    glUseProgram(shaderProgram);
//    doAfterLinking();
//}
//
//void Shader::disable() {
//    glUseProgram(0);
//}
//
//void Shader::logShader(GLuint shader) {
//    GLint logLength;
//    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
//    if (logLength > 0) {
//        auto *logBuffer = (char *) malloc(static_cast<size_t>(logLength));
//        GLsizei written;
//        glGetShaderInfoLog(shader, logLength, &written, logBuffer);
//        cerr << "Shader log: \n" << logBuffer << endl;
//        free(logBuffer);
//    }
//};
//
//void Shader::logProgram() {
//    GLint logLength;
//    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
//    if (logLength > 0) {
//        auto *logBuffer = (char *) malloc(static_cast<size_t>(logLength));
//        GLsizei written;
//        glGetProgramInfoLog(shaderProgram, logLength, &written, logBuffer);
//        cerr << "Program log: \n" << logBuffer << endl;
//        free(logBuffer);
//    }
//}
//
//void Shader::doAfterLinking() {
////    // 获得着色程序内的uniform block位置
////    GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, "BlobSettings");
////    // 按照uniform block的尺寸分配物理内存空间
////    GLint blockSize;
////    glGetActiveUniformBlockiv(shaderProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
////    GLubyte *blockBuffer = (GLubyte *) malloc(static_cast<size_t>(blockSize));
////    // 得到uniform block内变量相对于起点的偏移量
////    const GLchar *names[] = {"InnerColor", "OuterColor", "RadiusInner", "RadiusOuter"};
////    GLuint indices[4];
////    glGetUniformIndices(shaderProgram, 4, names, indices);
////    GLint offset[4];
////    glGetActiveUniformsiv(shaderProgram, 4, indices, GL_UNIFORM_OFFSET, offset);
////    // 把数据按合适的偏移塞到物理内存里
////    GLfloat outerColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
////    GLfloat innerColor[] = {1.0f, 1.0f, 0.75f, 1.0f};
////    GLfloat innerRadius = 0.25f;
////    GLfloat outerRadius = 0.45f;
////    memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat));
////    memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat));
////    memcpy(blockBuffer + offset[2], &innerRadius, 4 * sizeof(GLfloat));
////    memcpy(blockBuffer + offset[3], &outerRadius, 4 * sizeof(GLfloat));
////    // 在显存里创建缓存对象并把物理内存上的数据塞进去
////    GLuint uboHandle;
////    glGenBuffers(1, &uboHandle);
////    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
////    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);   // GL_DYNAMIC_DRAW指定优化方式
////    // 把着色程序内的uniform block和显存上的缓存对象绑定
////    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);
//}

Shader::Shader() : shaderProgram(0), linked(false) {}

Shader::~Shader() {
    if (shaderProgram == 0) {
        return;
    }

    // Query the number of attached shaders
    GLint numShaders = 0;
    glGetProgramiv(shaderProgram, GL_ATTACHED_SHADERS, &numShaders);

    // Get the shader names
    GLuint *shaderNames = new GLuint[numShaders];
    glGetAttachedShaders(shaderProgram, numShaders, nullptr, shaderNames);

    // Delete the shaders
    for (int i = 0; i < numShaders; i++)
        glDeleteShader(shaderNames[i]);

    // Delete the program
    glDeleteProgram(shaderProgram);

    delete[] shaderNames;
}

void Shader::compileShader(const char *fileName)
throw(GLSLProgramException) {
    int numExts = sizeof(GLSLShaderInfo::extensions) / sizeof(GLSLShaderInfo::shader_file_extension);

    // Check the file name's extension to determine the shader type
    string ext = getExtension(fileName);
    GLSLShader::GLSLShaderType type = GLSLShader::VERTEX;
    bool matchFound = false;
    for (int i = 0; i < numExts; i++) {
        if (ext == GLSLShaderInfo::extensions[i].ext) {
            matchFound = true;
            type = GLSLShaderInfo::extensions[i].type;
            break;
        }
    }

    // If we didn't find a match, throw an exception
    if (!matchFound) {
        string msg = "Unrecognized extension: " + ext;
        throw GLSLProgramException(msg);
    }

    // Pass the discovered shader type along
    compileShader(fileName, type);
}

string Shader::getExtension(const char *name) {
    string nameStr(name);

    size_t loc = nameStr.find_last_of('.');
    if (loc != string::npos) {
        return nameStr.substr(loc, string::npos);
    }
    return "";
}

void Shader::compileShader(const char *fileName,
                           GLSLShader::GLSLShaderType type)
throw(GLSLProgramException) {
    if (!fileExists(fileName)) {
        string message = string("Shader: ") + fileName + " not found.";
        throw GLSLProgramException(message);
    }

    if (shaderProgram <= 0) {
        shaderProgram = glCreateProgram();
        if (shaderProgram == 0) {
            throw GLSLProgramException("Unable to create shader program.");
        }
    }

    ifstream inFile(fileName, ios::in);
    if (!inFile) {
        string message = string("Unable to open: ") + fileName;
        throw GLSLProgramException(message);
    }

    // Get file contents
    stringstream code;
    code << inFile.rdbuf();
    inFile.close();

    compileShader(code.str(), type, fileName);
}

void Shader::compileShader(const string &source,
                           GLSLShader::GLSLShaderType type,
                           const char *fileName)
throw(GLSLProgramException) {
    if (shaderProgram <= 0) {
        shaderProgram = glCreateProgram();
        if (shaderProgram == 0) {
            throw GLSLProgramException("Unable to create shader program.");
        }
    }

    GLuint shaderHandle = glCreateShader(type);

    const char *c_code = source.c_str();
    glShaderSource(shaderHandle, 1, &c_code, nullptr);

    // Compile the shader
    glCompileShader(shaderHandle);

    // Check for errors
    int result;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
    if (GL_FALSE == result) {
        // Compile failed, get log
        int length = 0;
        string logString;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            char *c_log = new char[length];
            int written = 0;
            glGetShaderInfoLog(shaderHandle, length, &written, c_log);
            logString = c_log;
            delete[] c_log;
        }
        string msg;
        if (fileName) {
            msg = string(fileName) + ": shader compliation failed\n";
        } else {
            msg = "Shader compilation failed.\n";
        }
        msg += logString;

        throw GLSLProgramException(msg);

    } else {
        // Compile succeeded, attach shader
        glAttachShader(shaderProgram, shaderHandle);
    }
}

void Shader::link() throw(GLSLProgramException) {
    if (linked) return;
    if (shaderProgram <= 0)
        throw GLSLProgramException("Program has not been compiled.");

    glLinkProgram(shaderProgram);

    int status = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
        // Store log and return false
        int length = 0;
        string logString;

        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            char *c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(shaderProgram, length, &written, c_log);
            logString = c_log;
            delete[] c_log;
        }

        throw GLSLProgramException(string("Program link failed:\n") + logString);
    } else {
        uniformLocations.clear();
        linked = true;
    }
}

void Shader::use() throw(GLSLProgramException) {
    if (shaderProgram <= 0 || (!linked))
        throw GLSLProgramException("Shader has not been linked");
    glUseProgram(shaderProgram);
}

GLuint Shader::getProgram() {
    return shaderProgram;
}

void Shader::disable() {
    glUseProgram(0);
}

void Shader::bindAttribLocation(GLuint location, const char *name) {
    glBindAttribLocation(shaderProgram, location, name);
}

void Shader::bindFragDataLocation(GLuint location, const char *name) {
    glBindFragDataLocation(shaderProgram, location, name);
}

void Shader::setUniform(const char *name, float x, float y, float z) {
    GLint loc = getUniformLocation(name);
    glUniform3f(loc, x, y, z);
}

void Shader::setUniform(const char *name, const vec3 &v) {
    this->setUniform(name, v.x, v.y, v.z);
}

void Shader::setUniform(const char *name, const vec4 &v) {
    GLint loc = getUniformLocation(name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void Shader::setUniform(const char *name, const vec2 &v) {
    GLint loc = getUniformLocation(name);
    glUniform2f(loc, v.x, v.y);
}

void Shader::setUniform(const char *name, const mat4 &m) {
    GLint loc = getUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void Shader::setUniform(const char *name, const mat3 &m) {
    GLint loc = getUniformLocation(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void Shader::setUniform(const char *name, float val) {
    GLint loc = getUniformLocation(name);
    glUniform1f(loc, val);
}

void Shader::setUniform(const char *name, int val) {
    GLint loc = getUniformLocation(name);
    glUniform1i(loc, val);
}

void Shader::setUniform(const char *name, GLuint val) {
    GLint loc = getUniformLocation(name);
    glUniform1ui(loc, val);
}

void Shader::setUniform(const char *name, bool val) {
    int loc = getUniformLocation(name);
    glUniform1i(loc, val);
}

void Shader::printActiveUniforms() {
#ifdef __APPLE__
    // For OpenGL 4.1, use glGetActiveUniform
    GLint nUniforms, size, location, maxLen;
    GLchar * name;
    GLsizei written;
    GLenum type;

    glGetProgramiv( handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
    glGetProgramiv( handle, GL_ACTIVE_UNIFORMS, &nUniforms);

    name = new GLchar[ maxLen ];

    printf("Active uniforms:\n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nUniforms; ++i ) {
        glGetActiveUniform( handle, i, maxLen, &written, &size, &type, name );
        location = glGetUniformLocation(handle, name);
        printf(" %-5d %s (%s)\n", location, name, getTypeString(type));
    }

    delete [] name;
#else
    // For OpenGL 4.3 and above, use glGetProgramResource
    GLint numUniforms = 0;
    glGetProgramInterfaceiv(shaderProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

    GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX};

    printf("Active uniforms:\n");
    for (int i = 0; i < numUniforms; ++i) {
        GLint results[4];
        glGetProgramResourceiv(shaderProgram, GL_UNIFORM, i, 4, properties, 4, NULL, results);

        if (results[3] != -1) continue;  // Skip uniforms in blocks
        GLint nameBufSize = results[0] + 1;
        char *name = new char[nameBufSize];
        glGetProgramResourceName(shaderProgram, GL_UNIFORM, i, nameBufSize, NULL, name);
        printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
        delete[] name;
    }
#endif
}

void Shader::printActiveUniformBlocks() {
#ifdef __APPLE__
    // For OpenGL 4.1, use glGetActiveUniformBlockiv
    GLint written, maxLength, maxUniLen, nBlocks, binding;
    GLchar * name;

    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxLength);
    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_BLOCKS, &nBlocks);
    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniLen);
    GLchar * uniName = new GLchar[maxUniLen];
    name = new GLchar[maxLength];

    printf("Active Uniform blocks: \n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nBlocks; i++ ) {
        glGetActiveUniformBlockName( handle, i, maxLength, &written, name );
        glGetActiveUniformBlockiv( handle, i, GL_UNIFORM_BLOCK_BINDING, &binding);
        printf("Uniform block \"%s\" (%d):\n", name, binding);

        GLint nUnis;
        glGetActiveUniformBlockiv( handle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &nUnis);
        GLint * unifIndexes = new GLint[nUnis];
        glGetActiveUniformBlockiv( handle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, unifIndexes);

        for( int unif = 0; unif < nUnis; ++unif ) {
          GLint uniIndex = unifIndexes[unif];
          GLint size;
          GLenum type;

          glGetActiveUniform( handle, uniIndex, maxUniLen, &written, &size, &type, uniName );
          printf("    %s (%s)\n", name, getTypeString(type));
        }

        delete [] unifIndexes;
    }
    delete [] name;
    delete [] uniName;
#else
    GLint numBlocks = 0;

    glGetProgramInterfaceiv(shaderProgram, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
    GLenum blockProps[] = {GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH};
    GLenum blockIndex[] = {GL_ACTIVE_VARIABLES};
    GLenum props[] = {GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX};

    for (int block = 0; block < numBlocks; ++block) {
        GLint blockInfo[2];
        glGetProgramResourceiv(shaderProgram, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, NULL, blockInfo);
        GLint numUnis = blockInfo[0];

        char *blockName = new char[blockInfo[1] + 1];
        glGetProgramResourceName(shaderProgram, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, NULL, blockName);
        printf("Uniform block \"%s\":\n", blockName);
        delete[] blockName;

        GLint *unifIndexes = new GLint[numUnis];
        glGetProgramResourceiv(shaderProgram, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnis, NULL, unifIndexes);

        for (int unif = 0; unif < numUnis; ++unif) {
            GLint uniIndex = unifIndexes[unif];
            GLint results[3];
            glGetProgramResourceiv(shaderProgram, GL_UNIFORM, uniIndex, 3, props, 3, NULL, results);

            GLint nameBufSize = results[0] + 1;
            char *name = new char[nameBufSize];
            glGetProgramResourceName(shaderProgram, GL_UNIFORM, uniIndex, nameBufSize, NULL, name);
            printf("    %s (%s)\n", name, getTypeString(results[1]));
            delete[] name;
        }

        delete[] unifIndexes;
    }
#endif
}

void Shader::printActiveAttribs() {
#ifdef __APPLE__
    // For OpenGL 4.1, use glGetActiveAttrib
    GLint written, size, location, maxLength, nAttribs;
    GLenum type;
    GLchar * name;

    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &nAttribs);

    name = new GLchar[maxLength];
    printf("Active Attributes: \n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nAttribs; i++ ) {
        glGetActiveAttrib( handle, i, maxLength, &written, &size, &type, name );
        location = glGetAttribLocation(handle, name);
        printf(" %-5d %s (%s)\n", location, name, getTypeString(type));
    }
    delete [] name;
#else
    // >= OpenGL 4.3, use glGetProgramResource
    GLint numAttribs;
    glGetProgramInterfaceiv(shaderProgram, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

    GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};

    printf("Active attributes:\n");
    for (int i = 0; i < numAttribs; ++i) {
        GLint results[3];
        glGetProgramResourceiv(shaderProgram, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

        GLint nameBufSize = results[0] + 1;
        char *name = new char[nameBufSize];
        glGetProgramResourceName(shaderProgram, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
        printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
        delete[] name;
    }
#endif
}

const char *Shader::getTypeString(GLenum type) {
    // There are many more types than are covered here, but
    // these are the most common in these examples.
    switch (type) {
        case GL_FLOAT:
            return "float";
        case GL_FLOAT_VEC2:
            return "vec2";
        case GL_FLOAT_VEC3:
            return "vec3";
        case GL_FLOAT_VEC4:
            return "vec4";
        case GL_DOUBLE:
            return "double";
        case GL_INT:
            return "int";
        case GL_UNSIGNED_INT:
            return "unsigned int";
        case GL_BOOL:
            return "bool";
        case GL_FLOAT_MAT2:
            return "mat2";
        case GL_FLOAT_MAT3:
            return "mat3";
        case GL_FLOAT_MAT4:
            return "mat4";
        default:
            return "?";
    }
}

int Shader::getUniformLocation(const char *name) {
    map<string, int>::iterator pos;
    pos = uniformLocations.find(name);

    if (pos == uniformLocations.end()) {
        uniformLocations[name] = glGetUniformLocation(shaderProgram, name);
    }

    return uniformLocations[name];
}

bool Shader::fileExists(const string &fileName) {
    struct stat info;
    int ret = -1;

    ret = stat(fileName.c_str(), &info);
    return 0 == ret;
}


