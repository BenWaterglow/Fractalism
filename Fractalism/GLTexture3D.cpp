#include "GLTexture3D.hpp"
#include "Exceptions.hpp"

GLTexture3D::GLTexture3D(GLsizei width, GLsizei height, GLsizei depth) {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_3D, id);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_3D, 0);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    throw GLError("Could not create 3D OpenGL texture: %s (%d)", glewGetErrorString(err), err);
  }
}

GLTexture3D::GLTexture3D(GLTexture3D&& other) noexcept : id(std::exchange(other.id, 0)) {}

GLTexture3D::~GLTexture3D() {
  glDeleteTextures(1, &id);
}

GLTexture3D::operator GLuint() const noexcept {
  return id;
}

void GLTexture3D::clear() const {
  glClearTexImage(id, 0, GL_RGBA, GL_BYTE, nullptr);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    throw GLError("Could not clear 3D OpenGL texture: %s (%d)", glewGetErrorString(err), err);
  }
}

cl::ImageGL GLTexture3D::operator()(const cl::Context& ctx) const {
  try {
    return cl::ImageGL(ctx, CL_MEM_READ_WRITE, GL_TEXTURE_3D, 0, id);
  }
  catch (const cl::Error& e) {
    throw CLError("Could not create OpenCL/OpenGL image", e);
  }
}

