#ifndef _GL_UTILS_HPP_
#define _GL_UTILS_HPP_

#include <GL/glew.h>

#include "Exceptions.hpp"

namespace glutils {
  inline void checkGLError() {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      throw GLError("GL error: %s (%d)", glewGetErrorString(err), err);
    }
  }
}

#endif // !_GL_UTILS_HPP_
