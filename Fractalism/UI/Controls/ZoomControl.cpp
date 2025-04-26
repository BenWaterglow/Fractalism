#include <Fractalism/UI/Controls/ZoomControl.hpp>

#include <limits>
#include <cmath>

#include <Fractalism/Events.hpp>

namespace fractalism::ui::controls {
  namespace {
    static const real lnMax(std::log(gpu::types::Viewspace::maxZoom));
    static const real lnMin(std::log(gpu::types::Viewspace::minZoom));
    static const unsigned int sliderSteps(100);

    static int zoomToInt(real zoom) {
      static const real modifier = real(sliderSteps) / (lnMax - lnMin);
      return (std::log(zoom) - lnMin) * modifier;
    }

    static real intToZoom(int value) {
      return gpu::types::Viewspace::minZoom * std::exp((real(value)/real(sliderSteps))*(lnMax - lnMin));
    }
  }
      
  ZoomControl::ZoomControl(wxWindow& parent, real& zoom) :
      wxControl(&parent, wxID_ANY),
      slider(*new wxSlider(
        this,
        wxID_ANY,
        zoomToInt(zoom),
        zoomToInt(gpu::types::Viewspace::minZoom),
        zoomToInt(gpu::types::Viewspace::maxZoom))),
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