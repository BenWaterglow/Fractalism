#ifndef _FRACTALISM_GL_TEXTURE_3D_HPP_
#define _FRACTALISM_GL_TEXTURE_3D_HPP_

#include <GL/glew.h>
#include <Fractalism/GPU/OpenCL/CLCommon.hpp>

namespace fractalism::gpu::opengl {
  class GLTexture3D {
  public:
    GLTexture3D() = default;
    GLTexture3D(cl::NDRange& range);
    GLTexture3D(GLTexture3D&& other) noexcept;
    ~GLTexture3D();
    GLTexture3D& operator=(GLTexture3D&& other) noexcept;
    operator GLuint() const noexcept;
    operator cl::ImageGL() const;
    void resize(cl::NDRange& range);
    void clear() const;
    void free();
  private:
    GLuint id;
  };
}
#endif