#ifndef _FRACTALISM_GL_TEXTURE_3D_HPP_
#define _FRACTALISM_GL_TEXTURE_3D_HPP_

#include <Fractalism/GPU/OpenCL/CLCommon.hpp>
#include <GL/glew.h>

namespace fractalism::gpu::opengl {

/**
 * @class GLTexture3D
 * @brief Manages a 3D OpenGL texture.
 */
class GLTexture3D {
public:
  /**
   * @brief Default constructor.
   */
  GLTexture3D() = default;

  /**
   * @brief Constructs a GLTexture3D with the specified range.
   * @param range The OpenCL NDRange specifying the texture dimensions.
   */
  GLTexture3D(cl::NDRange& range);

  /**
   * @brief Move constructor.
   * @param other The other GLTexture3D to move from.
   */
  GLTexture3D(GLTexture3D&& other) noexcept;

  /**
   * @brief Destructor that cleans up the texture.
   */
  ~GLTexture3D();

  /**
   * @brief Move assignment operator.
   * @param other The other GLTexture3D to move from.
   * @return Reference to this GLTexture3D.
   */
  GLTexture3D& operator=(GLTexture3D&& other) noexcept;

  /**
   * @brief Implicit conversion to GLuint.
   * @return The OpenGL texture ID.
   */
  operator GLuint() const noexcept;

  /**
   * @brief Implicit conversion to cl::ImageGL.
   * @return The OpenCL image associated with the OpenGL texture.
   */
  operator cl::ImageGL() const;

  /**
   * @brief Resizes the texture to the specified range.
   * @param range The OpenCL NDRange specifying the new texture dimensions.
   */
  void resize(cl::NDRange& range);

  /**
   * @brief Clears the texture.
   */
  void clear() const;

  /**
   * @brief Frees the texture resources.
   */
  void free();

private:
  GLuint id; ///< The OpenGL texture ID.
};
} // namespace fractalism::gpu::opengl

#endif
