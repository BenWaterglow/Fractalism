#ifndef _FRACTALISM_GL_UNIFORM_HPP_
#define _FRACTALISM_GL_UNIFORM_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fractalism::gpu::opengl {

/**
 * @brief Template specialization for OpenGL uniform variables.
 * @tparam id The location of the uniform variable.
 * @tparam T The type of the uniform variable.
 */
template <GLint id, typename T> struct GLUniform {};

/**
 * @brief Macro to declare a GLUniform type for a specific OpenGL uniform type.
 * @param T The type of the uniform variable.
 * @param fn The OpenGL function to set the uniform variable.
 * @param size The size of the uniform variable.
 */
#define DECLARE_UNIFORM_TYPE(T, fn, size)                                      \
  template <GLint id> struct GLUniform<id, T> {                                \
    /**                                                                        \
     * @brief Template for the next uniform variable in the sequence.          \
     * @tparam V The type of the next uniform variable.                        \
     */                                                                        \
    template <typename V> static constexpr GLUniform<id + size, V> next = {};  \
    /**                                                                        \
     * @brief Sets the uniform variable to the specified value.                \
     * @param value The value to set the uniform variable to.                  \
     * @return Reference to this GLUniform.                                    \
     */                                                                        \
    inline auto operator=(const T&& value) const { fn; return *this; }         \
    /**                                                                        \
     * @brief Sets the uniform variable to the specified value.                \
     * @param value The value to set the uniform variable to.                  \
     * @return Reference to this GLUniform.                                    \
     */                                                                        \
    inline auto operator=(const T& value) const { fn; return *this; }          \
  }

DECLARE_UNIFORM_TYPE(int, glUniform1i(id, value), 1);
DECLARE_UNIFORM_TYPE(float, glUniform1f(id, value), 1);
DECLARE_UNIFORM_TYPE(glm::vec2, glUniform2f(id, value.x, value.y), 1);
DECLARE_UNIFORM_TYPE(glm::vec3, glUniform3f(id, value.x, value.y, value.z), 1);
DECLARE_UNIFORM_TYPE(glm::vec4, glUniform4f(id, value.x, value.y, value.z, value.w), 1);
DECLARE_UNIFORM_TYPE(glm::mat3, glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(value)), 1);
DECLARE_UNIFORM_TYPE(glm::mat4, glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value)), 1);
} // namespace fractalism::gpu::opengl

#endif // end include guard
