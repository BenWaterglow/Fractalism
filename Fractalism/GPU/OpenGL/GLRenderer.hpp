#ifndef _FRACTALISM_GL_RENDERER_HPP_
#define _FRACTALISM_GL_RENDERER_HPP_

#include <GL/glew.h>
#include <Fractalism/UI/UICommon.hpp>
#include <wx/glcanvas.h>

#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism::gpu::opengl {
  class GLRenderer {
  public:
    GLRenderer();
    ~GLRenderer();
    void render(ViewWindowSettings& settings, wxGLCanvas& canvas, GLuint texture) const;
  private:
    GLuint VBOs[4];
    GLuint VAOs[2];
  };
}
#endif