#ifndef _FRACTALISM_RENDER_PANEL_HPP_
#define _FRACTALISM_RENDER_PANEL_HPP_

#include <Fractalism/UI/WxIncludeHelper.hpp>
#include <Fractalism/UI/GLRenderCanvas.hpp>

namespace fractalism {
  namespace ui {
    class RenderPanel : public wxPanel {
    public:
      RenderPanel(wxWindow& parent, options::Space space);
      inline ~RenderPanel() override {}
      inline operator wxGLCanvas& () const {
        return canvas;
      }
    private:
      GLRenderCanvas& canvas;
    };
  }
}
#endif