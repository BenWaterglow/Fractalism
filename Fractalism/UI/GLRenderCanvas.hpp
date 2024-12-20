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
      glm::dvec2 lastPoint;
      const wxEventTypeTag<StateChangeEvent>& viewChangeEvent;
    };
  }
}

#endif