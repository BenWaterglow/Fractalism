#include <Fractalism/UI/Controls/ViewspaceToolBar.hpp>

namespace fractalism::ui::controls {
  namespace {
    namespace types = gpu::types;
  }
  ViewspaceToolBar::ViewspaceToolBar(wxWindow & parent, types::Viewspace & viewspace) :
        wxAuiToolBar(&parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_TEXT | wxAUI_TB_VERTICAL),
        view(viewspace),
        numberInput(*new HypercomplexNumberControl(*this, "Center", viewspace.center)),
        viewMapping(*new ViewMappingControl(*this, "View Axis Mapping", viewspace.mapping)),
        zoom(*new ZoomControl(*this, viewspace.zoom)) {
    AddControl(&numberInput, "Center");
    AddControl(&viewMapping, "View Axis Mapping");
    AddControl(&zoom, "Zoom");
    updateCenter();
    updateViewMapping();
    updateZoom();
    updateNumberSystem();
    updateRenderDimensions();
    Realize();
  }

  void ViewspaceToolBar::updateCenter() {
    numberInput.updateNumber();
  }

  void ViewspaceToolBar::updateViewMapping() {
    viewMapping.updateViewMapping();
  }

  void ViewspaceToolBar::updateZoom() {
    zoom.updateZoom();
  }

  void ViewspaceToolBar::updateNumberSystem() {
    numberInput.updateNumberSystem();
    viewMapping.updateNumberSystem();
  }

  void ViewspaceToolBar::updateRenderDimensions() {
    viewMapping.updateRenderDimensions();
  }
}