#ifndef _GL_SHADER_HPP_
#define _GL_SHADER_HPP_

#include <GL/glew.h>

class GLShader {
public:
  GLShader(GLenum type, const char *filename);
  ~GLShader();
  operator GLuint() const;
  GLuint id;
};

#endif