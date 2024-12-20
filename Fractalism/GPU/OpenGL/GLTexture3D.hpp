#ifndef _FRACTALISM_GL_TEXTURE_3D_HPP_
#define _FRACTALISM_GL_TEXTURE_3D_HPP_

#include <GL/glew.h>
#include <Fractalism/GPU/OpenCL/CLIncludeHelper.hpp>

namespace fractalism {
  namespace gpu {
    namespace opengl {

      class GLTexture3D {
      public:
        GLTexture3D(GLsizei width, GLsizei height, GLsizei depth);
        GLTexture3D(GLTexture3D&& other) noexcept;
        ~GLTexture3D();
        GLTexture3D& operator=(GLTexture3D&& other) noexcept;
        operator GLuint() const noexcept;
        void clear() const;
        void free();
        cl::ImageGL operator()(const cl::Context& ctx) const;
      private:
        GLuint id;
      };
    }
  }
}
#endif