#ifndef _FRACTALISM_GL_RENDER_CANVAS_HPP_
#define _FRACTALISM_GL_RENDER_CANVAS_HPP_

#include <GL/glew.h>
#include <Fractalism/UI/WxIncludeHelper.hpp>
#include <wx/glcanvas.h>

#include <glm/glm.hpp>

#include <Fractalism/GPU/OpenCL/CLTypes.hpp>
#include <Fractalism/Events.hpp>
#include <Fractalism/Proxy.hpp>

namespace fractalism {
  namespace ui {
    namespace {
      namespace cltypes = gpu::opencl::cltypes;
    }
    class GLRenderCanvas : public wxGLCanvas {
    public:
      GLRenderCanvas(wxWindow& parent, options::Space space, wxStatusBar& statusBar);
    private:
      void bindMouseHandler(
        const wxEventTypeTag<wxMouseEvent>& eventType,
        proxy::Proxy<cltypes::Viewspace> auto& viewspace);
      void handleMouseEvents(double x, double y, wxMouseEvent& evt, proxy::Proxy<cltypes::Viewspace> auto& viewspace);
      wxStatusBar& statusBar;
      glm::dvec2 lastPoint;
      const wxEventTypeTag<StateChangeEvent>& viewChangeEvent;
    };
  }
}

#endif