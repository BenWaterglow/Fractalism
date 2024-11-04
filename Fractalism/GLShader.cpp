#include "GLShader.hpp"
#include "Utils.hpp"
#include "Exceptions.hpp"

GLShader::GLShader(GLenum type, const char *filename) : id(glCreateShader(type)) {
  std::unique_ptr<char[]> fileSource = utils::readFile(filename);
  const char *sourceString = fileSource.get();
  glShaderSource(id, 1, &sourceString, nullptr);

  glCompileShader(id);
  GLint param;
  glGetShaderiv(id, GL_COMPILE_STATUS, &param);
  if (!param) {
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &param);
    std::unique_ptr<char[]> infoLog = std::make_unique<char[]>(param);
    glGetShaderInfoLog(id, param, nullptr, infoLog.get());
    throw GLCompileError("Could not compile OpenGL shader %s. Compiler log:\n%s", filename, infoLog.get());
  }
}

GLShader::~GLShader() {
  glDeleteShader(id);
}

GLShader::operator GLuint() const {
  return id;
}