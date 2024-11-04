#include <memory>

#include "GLShaderProgram.hpp"
#include "GLShader.hpp"
#include "GLUtils.hpp"
#include "Exceptions.hpp"

GLShaderProgram::GLShaderProgram() : GLShaderProgram(nullptr, nullptr) {}

GLShaderProgram::GLShaderProgram(const char *vertexShaderFile, const char *fragmentShaderFile) :
    id(0),
    vertexShaderFile(vertexShaderFile),
    fragmentShaderFile(fragmentShaderFile) {}

GLShaderProgram& GLShaderProgram::operator=(GLShaderProgram&& other) noexcept {
  if (id != other.id) {
    glDeleteProgram(id);
  }
  id = std::exchange(other.id, 0);
  vertexShaderFile = std::move(other.vertexShaderFile);
  fragmentShaderFile = std::move(other.fragmentShaderFile);
  return *this;
}

GLShaderProgram::~GLShaderProgram() {
  glDeleteProgram(id);
}

void GLShaderProgram::load() {
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  bool isCurrent = currentProgram == id;
  if (isCurrent) {
    glUseProgram(0);
  }
  glDeleteProgram(id);
  id = glCreateProgram();

  GLShader vertex(GL_VERTEX_SHADER, vertexShaderFile);
  GLShader fragment(GL_FRAGMENT_SHADER, fragmentShaderFile);

  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);

  GLint param;
  glGetProgramiv(id, GL_LINK_STATUS, &param);
  if (!param) {
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &param);
    std::unique_ptr<char[]> infoLog = std::make_unique<char[]>(param);
    glGetProgramInfoLog(id, param, nullptr, infoLog.get());
    throw GLLinkError("Could not link OpenGL program for shaders %s and %s. Info log:\n%s", vertexShaderFile, fragmentShaderFile, infoLog.get());
  }

  if (isCurrent) {
    glUseProgram(id);
  }
  glutils::checkGLError();
}
