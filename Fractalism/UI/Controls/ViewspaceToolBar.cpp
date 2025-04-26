#include <Fractalism/UI/Controls/ViewspaceToolBar.hpp>

namespace fractalism::ui::controls {
  namespace {
    namespace types = gpu::types;
  }
  ViewspaceToolBar::ViewspaceToolBar(wxWindow & parent, types::Viewspace & viewspace) :
        wxAuiToolBar(&parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_TEXT | wxAUI_TB_VERTICAL),
        view(viewspace),
        numberInput(*new HypercomplexNumberControl(*this, viewspace.center)),
        viewMapping(*new ViewMappingControl(*this, viewspace.mapping)),
        zoom(*new ZoomControl(*this, viewspace.zoom)) {
    AddLabel(numberInput.GetId(), "Center");
    AddControl(&numberInput);
    AddLabel(viewMapping.GetId(), "View Axis Mapping");
    AddControl(&viewMapping);
    AddLabel(zoom.GetId(), "Zoom");
    AddControl(&zoom);
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