#ifndef _FRACTALISM_GL_RENDERER_HPP_
#define _FRACTALISM_GL_RENDERER_HPP_

#include <GL/glew.h>
#include <Fractalism/UI/WxIncludeHelper.hpp>
#include <wx/glcanvas.h>

#include <Fractalism/GPU/OpenGL/GLShaderProgram.hpp>
#include <Fractalism/Options.hpp>

namespace fractalism {
  namespace gpu {
    namespace opengl {

      class GLRenderer {
      public:
        GLRenderer();
        ~GLRenderer();
        void render(wxGLCanvas& canvas, GLuint texture) const;
      private:
        GLuint VBOs[4];
        GLuint VAOs[2];
      };
    }
  }
}
#endif