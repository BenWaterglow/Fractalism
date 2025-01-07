#ifndef _FRACTALISM_GL_RENDER_CANVAS_HPP_
#define _FRACTALISM_GL_RENDER_CANVAS_HPP_

#include <GL/glew.h>
#include <Fractalism/UI/UICommon.hpp>
#include <wx/glcanvas.h>

#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism::ui {
  class GLRenderCanvas : public wxGLCanvas {
  public:
    GLRenderCanvas(wxWindow& parent, ViewWindowSettings& settings, wxStatusBar& statusBar);
  private:
    gpu::types::Coordinates lastPoint;
    void setLastPoint(const gpu::types::Coordinates& coordinates);
  };
}

#endif