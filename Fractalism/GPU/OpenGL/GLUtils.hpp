#ifndef _FRACTALISM_GL_UTILS_HPP_
#define _FRACTALISM_GL_UTILS_HPP_

#include <GL/glew.h>
#include <string>

#include <Fractalism/Exceptions.hpp>

namespace fractalism::gpu::opengl::glutils {

/**
 * @brief Checks for OpenGL errors and throws an exception if any are found.
 * @throws GLError if an OpenGL error is detected.
 */
inline void checkGLError() {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    throw GLError(err);
  }
}

/**
 * @brief Retrieves the compiler log for a given shader.
 * @param shaderId The ID of the shader.
 * @return The compiler log as a string.
 */
inline std::string getGlCompilerLog(GLuint shaderId) noexcept {
  GLint infoLogLength;
  glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

  std::string infoLog;
  infoLog.resize(infoLogLength);

  glGetShaderInfoLog(shaderId, infoLogLength, nullptr, infoLog.data());
  return infoLog;
}

/**
 * @brief Gets the shader type as a string.
 * @param type The GLenum representing the shader type.
 * @return The shader type as a string.
 */
inline std::string getShaderType(GLenum type) {
  switch (type) {
  case GL_VERTEX_SHADER:
    return "vertex";
  case GL_FRAGMENT_SHADER:
    return "fragment";
  default:
    return "";
  }
}

/**
 * @brief Retrieves the linker log for a given program.
 * @param programId The ID of the program.
 * @return The linker log as a string.
 */
static inline std::string getLinkerLog(GLuint programId) {
  GLint infoLogLength;
  glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

  std::string infoLog;
  infoLog.resize(infoLogLength);

  glGetProgramInfoLog(programId, infoLogLength, nullptr, infoLog.data());
  return infoLog;
}
} // namespace fractalism::gpu::opengl::glutils

#endif // !_GL_UTILS_HPP_
