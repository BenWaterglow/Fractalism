#include <Fractalism/GPU/OpenGL/GLShader.hpp>

#include <string>

#include <Fractalism/Utils.hpp>
#include <Fractalism/Exceptions.hpp>

namespace fractalism {
  namespace gpu {
    namespace opengl {
      GLShader::GLShader(GLenum type, const char* filename) : id(glCreateShader(type)) {
        std::string contents = utils::readFile(filename);
        const char* fileSource = contents.c_str();
        glShaderSource(id, 1, &fileSource, nullptr);

        glCompileShader(id);
        GLint compileStatus;
        glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
        if (!compileStatus) {
          throw GLCompileError(type, id);
        }
      }

      GLShader::~GLShader() {
        glDeleteShader(id);
      }

      GLShader::operator GLuint() const {
        return id;
      }
    }
  }
}