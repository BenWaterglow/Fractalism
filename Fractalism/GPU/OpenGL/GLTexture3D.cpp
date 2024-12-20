#include <Fractalism/GPU/OpenGL/GLTexture3D.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/Exceptions.hpp>

namespace fractalism {
  namespace gpu {
    namespace opengl {

      GLTexture3D::GLTexture3D(GLsizei width, GLsizei height, GLsizei depth) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_3D, id);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_3D, 0);
        glutils::checkGLError();
      }

      GLTexture3D::GLTexture3D(GLTexture3D && other) noexcept : id(std::exchange(other.id, 0)) {}

      GLTexture3D::~GLTexture3D() {
        glDeleteTextures(1, &id);
      }

      GLTexture3D& GLTexture3D::operator=(GLTexture3D && other) noexcept {
        glDeleteTextures(1, &id);
        id = std::exchange(other.id, 0);
        return *this;
      }

      GLTexture3D::operator GLuint() const noexcept {
        return id;
      }

      void GLTexture3D::clear() const {
        glClearTexImage(id, 0, GL_RGBA, GL_BYTE, nullptr);
        glutils::checkGLError();
      }

      void GLTexture3D::free() {
        glDeleteTextures(1, &id);
        id = 0;
      }

      cl::ImageGL GLTexture3D::operator()(const cl::Context & ctx) const {
        try {
          return cl::ImageGL(ctx, CL_MEM_READ_WRITE, GL_TEXTURE_3D, 0, id);
        }
        catch (const cl::Error& e) {
          throw CLError("Could not create OpenCL/OpenGL image", e);
        }
      }
    }
  }
}