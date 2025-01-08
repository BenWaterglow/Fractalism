#ifndef _FRACTALISM_GL_SHADER_HPP_
#define _FRACTALISM_GL_SHADER_HPP_

#include <GL/glew.h>

namespace fractalism::gpu::opengl {

/**
 * @class GLShader
 * @brief Manages an OpenGL shader.
 */
class GLShader {
public:
  /**
   * @brief Constructs a GLShader from a file.
   * @param type The type of the shader (e.g., GL_VERTEX_SHADER,
   * GL_FRAGMENT_SHADER).
   * @param filename The path to the shader source file.
   */
  GLShader(GLenum type, const char* filename);

  /**
   * @brief Destructor that cleans up the shader.
   */
  ~GLShader();

  /**
   * @brief Implicit conversion to GLuint.
   * @return The OpenGL shader ID.
   */
  operator GLuint() const;

  GLuint id; ///< The OpenGL shader ID.
};
} // namespace fractalism::gpu::opengl

#endif
