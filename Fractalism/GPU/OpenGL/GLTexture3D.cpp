#include <Fractalism/GPU/OpenGL/GLTexture3D.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/Exceptions.hpp>
#include <Fractalism/App.hpp>

namespace fractalism::gpu::opengl {
  GLTexture3D::GLTexture3D(cl::NDRange& range) : id(0) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_3D, id);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, range[0], range[1], range[2], 0, GL_RGBA, GL_BYTE, nullptr);
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

  GLTexture3D::operator cl::ImageGL() const {
    try {
      return cl::ImageGL(App::get<GPUContext>(), CL_MEM_READ_WRITE, GL_TEXTURE_3D, 0, id);
    } catch (const cl::Error &e) {
      throw CLError("Could not create OpenCL/OpenGL image", e);
    }
  }

  void GLTexture3D::resize(cl::NDRange& range) {
    free(); // These textures can get quite large. Don't hog more VRAM than absolutely needed.
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_3D, id);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, range[0], range[1], range[2], 0, GL_RGBA, GL_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);
    glutils::checkGLError();
  }

  void GLTexture3D::clear() const {
    glClearTexImage(id, 0, GL_RGBA, GL_BYTE, nullptr);
    glutils::checkGLError();
  }

  void GLTexture3D::free() {
    glDeleteTextures(1, &id);
    id = 0;
  }
}