#ifndef _FRACTALISM_GL_SHADER_PROGRAM_HPP_
#define _FRACTALISM_GL_SHADER_PROGRAM_HPP_

#include <GL/glew.h>

namespace fractalism::gpu::opengl {

/**
 * @class GLShaderProgram
 * @brief Manages an OpenGL shader program.
 */
class GLShaderProgram {
public:
  /**
   * @brief Constructs an empty GLShaderProgram.
   */
  GLShaderProgram();

  /**
   * @brief Constructs a GLShaderProgram with vertex and fragment shaders.
   * @param vertexShaderFile The path to the vertex shader source file.
   * @param fragmentShaderFile The path to the fragment shader source file.
   */
  GLShaderProgram(const char* vertexShaderFile, const char* fragmentShaderFile);

  /**
   * @brief Destructor that cleans up the shader program.
   */
  ~GLShaderProgram();

  /**
   * @brief Move assignment operator.
   * @param other The other GLShaderProgram to move from.
   * @return Reference to this GLShaderProgram.
   */
  GLShaderProgram& operator=(GLShaderProgram&& other) noexcept;

  /**
   * @brief Implicit conversion to GLuint.
   * @return The OpenGL shader program ID.
   */
  operator auto() const { return id; }

  /**
   * @brief Loads and compiles the shader program.
   */
  void load();

private:
  GLuint id;                      ///< The OpenGL shader program ID.
  const char* vertexShaderFile;   ///< The path to the vertex shader source file.
  const char* fragmentShaderFile; ///< The path to the fragment shader source file.
};
} // namespace fractalism::gpu::opengl

#endif
