#ifndef _FRACTALISM_GL_SHADER_HPP_
#define _FRACTALISM_GL_SHADER_HPP_

#include <GL/glew.h>

namespace fractalism::gpu::opengl {
  class GLShader {
  public:
    GLShader(GLenum type, const char* filename);
    ~GLShader();
    operator GLuint() const;
    GLuint id;
  };
}
#endif