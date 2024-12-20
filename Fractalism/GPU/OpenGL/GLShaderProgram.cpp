#include <memory>

#include <Fractalism/GPU/OpenGL/GLShaderProgram.hpp>
#include <Fractalism/GPU/OpenGL/GLShader.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/Exceptions.hpp>

namespace fractalism {
  namespace gpu {
    namespace opengl {
      GLShaderProgram::GLShaderProgram() : GLShaderProgram(nullptr, nullptr) {}

      GLShaderProgram::GLShaderProgram(const char* vertexShaderFile, const char* fragmentShaderFile) :
        id(0),
        vertexShaderFile(vertexShaderFile),
        fragmentShaderFile(fragmentShaderFile) {
      }

      GLShaderProgram& GLShaderProgram::operator=(GLShaderProgram && other) noexcept {
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

        GLint linkStatus;
        glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus) {
          throw GLLinkError(id);
        }

        if (isCurrent) {
          glUseProgram(id);
        }
        glutils::checkGLError();
      }
    }
  }
}