#ifndef _FRACTALISM_GL_SHADER_PROGRAM_HPP_
#define _FRACTALISM_GL_SHADER_PROGRAM_HPP_

#include <GL/glew.h>

namespace fractalism::gpu::opengl {
  class GLShaderProgram {
  public:
    GLShaderProgram();
    GLShaderProgram(const char* vertexShaderFile, const char* fragmentShaderFile);
    ~GLShaderProgram();
    GLShaderProgram& operator=(GLShaderProgram&& other) noexcept;
    operator auto() const { return id; }
    void load();
  private:
    GLuint id;
    const char* vertexShaderFile;
    const char* fragmentShaderFile;
  };
}
#endif