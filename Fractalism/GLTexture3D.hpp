#ifndef _GL_TEXTURE_3D_HPP_
#define _GL_TEXTURE_3D_HPP_

#include <GL/glew.h>
#include "CLIncludeHelper.hpp"

class GLTexture3D {
public:
  GLTexture3D(GLsizei width, GLsizei height, GLsizei depth);
  GLTexture3D(GLTexture3D&& other) noexcept;
  ~GLTexture3D();
  operator GLuint() const noexcept;
  void clear() const;
  cl::ImageGL operator()(const cl::Context& ctx) const;
private:
  GLuint id;
};

#endif