#ifndef _FRACTALISM_GL_UTILS_HPP_
#define _FRACTALISM_GL_UTILS_HPP_

#include <string>
#include <GL/glew.h>

#include <Fractalism/Exceptions.hpp>

namespace fractalism::gpu::opengl::glutils {
  inline void checkGLError() {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      throw GLError(err);
    }
  }

  inline std::string getGlCompilerLog(GLuint shaderId) noexcept {
    GLint infoLogLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::string infoLog;
    infoLog.resize(infoLogLength);

    glGetShaderInfoLog(shaderId, infoLogLength, nullptr, infoLog.data());
    return infoLog;
  }

  inline std::string getShaderType(GLenum type) {
    switch (type) {
    case GL_VERTEX_SHADER: return "vertex";
    case GL_FRAGMENT_SHADER: return "fragment";
    default: return "";
    }
  }

  static inline std::string getLinkerLog(GLuint programId) {
    GLint infoLogLength;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::string infoLog;
    infoLog.resize(infoLogLength);

    glGetProgramInfoLog(programId, infoLogLength, nullptr, infoLog.data());
    return infoLog;
  }
}
#endif // !_GL_UTILS_HPP_
