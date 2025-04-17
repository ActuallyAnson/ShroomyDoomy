/* !
@file    	ShaderManager.h
@author  	pghali@digipen.edu
@co-author	parminder.singh@digipen.edu
@date    	31/03/2024


** Not being used in code count in this project **

Note: The contents of this file must not be updated by students. Otherwise,
something that works for you will not work for me. If you want something to be
modified, updated, or altered and it is useful for the entire class, please
speak to me.

This file contains definitions of member functions of class GLShader.
Please see the class declaration for information about these functions.

*//*__________________________________________________________________________*/
#include "pch.h"
#include "ShaderManager.h"
#include "Log.h"


using namespace FruitLoops;
GLint
ShaderManager::GetUniformLocation(GLchar const* name) {
    GLint location = glGetUniformLocation(programHandle, name);
    if (location < 0) {
        std::exit(EXIT_FAILURE);
    }
    return location;
}

GLboolean
ShaderManager::CompileLinkValidate(std::vector<std::pair<GLenum, std::string>> vec, bool compileFromFile) {
    for (auto& elem : vec) {
        if (compileFromFile) {
            if (GL_FALSE == CompileShaderFromFile(elem.first, elem.second.c_str())) {
                return GL_FALSE;
            }
        }
        else {
            if (GL_FALSE == CompileShaderFromString(elem.first, elem.second.c_str())) {
                return GL_FALSE;
            }
        }
    }

    if (GL_FALSE == Link()) {
        return GL_FALSE;
    }
    if (GL_FALSE == Validate()) {
        return GL_FALSE;
    }

    return GL_TRUE;
}

GLboolean
ShaderManager::CompileShaderFromFile(GLenum shaderType, const std::string& Filename) {
    if (programHandle <= 0) {
        programHandle = glCreateProgram();
        if (0 == programHandle) {
            logError("Unable to create Program");
        }
    }

    std::ifstream shaderFile(Filename, std::ifstream::in);
    if (!shaderFile) {
        logError("Error opening file {}", Filename);
    }
    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    shaderFile.close();
    return CompileShaderFromString(shaderType, buffer.str());
}

GLboolean
ShaderManager::CompileShaderFromString(GLenum shaderType,
    const std::string& shaderSource) {
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    if (programHandle <= 0) {
        programHandle = glCreateProgram();
        
        if (0 == programHandle) {
            logError("Cannot create program handle!");
        }
    }
   
    GLuint shaderHandle = 0;

    switch (shaderType) {
    case VERTEX_SHADER: shaderHandle = glCreateShader(GL_VERTEX_SHADER); break;
    case FRAGMENT_SHADER: shaderHandle = glCreateShader(GL_FRAGMENT_SHADER); break;
    default:
        logError("Shader type not implemented");
    }

    // load shader source code into shader object
    GLchar const* shaderCode[] = { shaderSource.c_str() };

    glShaderSource(shaderHandle, 1, shaderCode, NULL);

    // compile the shader
    glCompileShader(shaderHandle);

    // check compilation status
    GLint status;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
    if (GL_FALSE == status) {
        logError("Vertex shader compilation failed");
        GLint logLength;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar* log = new GLchar[logLength];
            GLsizei writtenLogLength;
            glGetShaderInfoLog(shaderHandle, logLength, &writtenLogLength, log);
            logError(log);
            delete[] log;
        }
        return GL_FALSE;
    }
    else { // attach the shader to the program object
        glAttachShader(programHandle, shaderHandle);
        return GL_TRUE;
    }
}

GLboolean ShaderManager::Link() {
    if (GL_TRUE == isLinked) {
        return GL_TRUE;
    }
    if (programHandle <= 0) {
        return GL_FALSE;
    }

    glLinkProgram(programHandle); // link the various compiled shaders

    // verify the link status
    GLint lnkStatus;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &lnkStatus);
    if (GL_FALSE == lnkStatus) {
        logString = "Failed to link shader program\n";
        GLint logLength;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar* logStr = new GLchar[logLength];
            GLsizei writtenLogLength;
            glGetProgramInfoLog(programHandle, logLength, &writtenLogLength, logStr);
            logError(logStr);
            delete[] logStr;
        }
        return GL_FALSE;
    }
    return isLinked = GL_TRUE;
}

void ShaderManager::Use() {
    if (programHandle > 0 && isLinked == GL_TRUE) {
        glUseProgram(programHandle);
    }
}

void ShaderManager::UnUse() {
    glUseProgram(0);
}

GLboolean ShaderManager::Validate() {
    if (programHandle <= 0 || isLinked == GL_FALSE) {
        return GL_FALSE;
    }

    glValidateProgram(programHandle);
    GLint status;
    glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &status);
    if (GL_FALSE == status) {
        logString = "Failed to validate shader program for current OpenGL context\n";
        GLint logLength;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar* logStr = new GLchar[logLength];
            GLsizei writtenLogLength;
            glGetProgramInfoLog(programHandle, logLength, &writtenLogLength, logStr);
            logError(logStr);
            delete[] logStr;
        }
        return GL_FALSE;
    }
    else {
        return GL_TRUE;
    }
}

GLuint ShaderManager::GetHandle() const {
    return programHandle;
}

GLboolean ShaderManager::IsLinked() const {
    return isLinked;
}

std::string ShaderManager::GetLog() const {
    return logString;
}

void ShaderManager::BindAttribLocation(GLuint index, GLchar const* name) {
    glBindAttribLocation(programHandle, index, name);
}

void ShaderManager::BindFragDataLocation(GLuint color_number, GLchar const* name) {
    glBindFragDataLocation(programHandle, color_number, name);
}

void ShaderManager::SetUniform(GLchar const* name, GLboolean val) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform1i(loc, val);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, GLint val) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform1i(loc, val);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, GLfloat val) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform1f(loc, val);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, GLfloat x, GLfloat y) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform2f(loc, x, y);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform3f(loc, x, y, z);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void
ShaderManager::SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform4f(loc, x, y, z, w);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, glm::vec2 const& val) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform2f(loc, val.x, val.y);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, glm::vec3 const& val) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform3f(loc, val.x, val.y, val.z);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, glm::vec4 const& val) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniform4f(loc, val.x, val.y, val.z, val.w);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, glm::mat3 const& val) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}

void ShaderManager::SetUniform(GLchar const* name, glm::mat4 const& val) {
    GLint loc = glGetUniformLocation(programHandle, name);
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0][0]);
    }
    else {
        logError("Uniform variable {} doesn't exist", name);
    }
}


