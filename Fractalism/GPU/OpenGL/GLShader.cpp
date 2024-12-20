#include <Fractalism/GPU/OpenGL/GLShader.hpp>
#include <Fractalism/Utils.hpp>
#include <Fractalism/Exceptions.hpp>

namespace fractalism {
  namespace gpu {
    namespace opengl {
      GLShader::GLShader(GLenum type, const char* filename) : id(glCreateShader(type)) {
        std::unique_ptr<char[]> fileSource = utils::readFile(filename);
        const char* sourceString = fileSource.get();
        glShaderSource(id, 1, &sourceString, nullptr);

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