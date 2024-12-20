#ifndef _FRACTALISM_GL_UNIFORM_HPP_
#define _FRACTALISM_GL_UNIFORM_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fractalism {
  namespace gpu {
    namespace opengl {
      template<GLint id, typename T>
      struct GLUniform {};

      #define UNIFORM_TYPE(T, fn, size) \
      template<GLint id> \
      struct GLUniform<id, T> { \
        template<typename V> \
        static constexpr GLUniform<id + size, V> next = {}; \
        inline auto operator=(const T&& value) const { \
          fn; \
          return *this;\
        } \
        inline auto operator=(const T& value) const { \
          fn; \
          return *this;\
        } \
      }

      UNIFORM_TYPE(int, glUniform1i(id, value), 1);
      UNIFORM_TYPE(float, glUniform1f(id, value), 1);
      UNIFORM_TYPE(glm::vec2, glUniform2f(id, value.x, value.y), 1);
      UNIFORM_TYPE(glm::vec3, glUniform3f(id, value.x, value.y, value.z), 1);
      UNIFORM_TYPE(glm::vec4, glUniform4f(id, value.x, value.y, value.z, value.w), 1);
      UNIFORM_TYPE(glm::mat3, glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(value)), 1);
      UNIFORM_TYPE(glm::mat4, glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value)), 1);

      #undef UNIFORM_TYPE
    }
  }
}
#endif // !_GL_UNIFORM_HPP_
