#include <Fractalism/UI/Controls/ZoomControl.hpp>

#include <limits>
#include <cmath>

#include <Fractalism/Settings.hpp>
#include <Fractalism/Events.hpp>

namespace fractalism::ui::controls {
  namespace {
    static constexpr int zoomToInt(real zoom) {
      return (zoom / Settings::maxZoom) * static_cast<real>(std::numeric_limits<int>::max());
    }

    static constexpr real intToZoom(int value) {
      return ((static_cast<real>(value) / static_cast<real>(std::numeric_limits<int>::max())) * Settings::maxZoom);
    }
    // TODO: move this stuff to gpu::types::Viewspace
    static_assert(zoomToInt(Settings::zoom1x) == 0);
    static_assert(zoomToInt(Settings::maxZoom) == std::numeric_limits<int>::max());
    static_assert(intToZoom(std::numeric_limits<int>::max()) == Settings::maxZoom);
  }
      
  ZoomControl::ZoomControl(wxWindow& parent, real& zoom) :
      wxControl(&parent, wxID_ANY),
      slider(*new wxSlider(
        this,
        wxID_ANY,
        zoomToInt(zoom),
        zoomToInt(Settings::minZoom),
        zoomToInt(Settings::maxZoom))),
      zoom(zoom) {
    SetLabel("Zoom");
    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(&slider);
    slider.Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
      this->zoom = intToZoom(evt.GetInt());
      events::ZoomChanged::fire(this, this->zoom);
    });
    SetSizerAndFit(sizer);
  }

  void ZoomControl::updateZoom() {
    slider.SetValue(zoomToInt(zoom));
  }
}