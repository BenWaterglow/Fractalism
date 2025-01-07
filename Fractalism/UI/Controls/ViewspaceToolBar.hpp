#ifndef _FRACTALISM_VIEWSPACE_CONTROL_HPP_
#define _FRACTALISM_VIEWSPACE_CONTROL_HPP_

#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/auibar.h>
#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/UI/Controls/ViewMappingControl.hpp>
#include <Fractalism/UI/Controls/HypercomplexNumberControl.hpp>
#include <Fractalism/UI/Controls/ZoomControl.hpp>
#include <Fractalism/Events.hpp>

namespace fractalism::ui::controls {
  class ViewspaceToolBar : public wxAuiToolBar {
  public:
    ViewspaceToolBar(wxWindow& parent, gpu::types::Viewspace& viewspace);
    void updateCenter();
    void updateViewMapping();
    void updateZoom();
    void updateNumberSystem();
    void updateRenderDimensions();
  private:
    gpu::types::Viewspace& view;
    HypercomplexNumberControl& numberInput;
    ViewMappingControl& viewMapping;
    ZoomControl& zoom;
  };
}

#endif // !_FRACTALISM_VIEWSPACE_CONTROL_HPP_
