#ifndef _FRACTALISM_ZOOM_CONTROL_HPP_
#define _FRACTALISM_ZOOM_CONTROL_HPP_

#include <Fractalism/UI/UICommon.hpp>

#include <Fractalism/GPU/Types.hpp>

namespace fractalism::ui::controls {
  class ZoomControl : public wxControl {
  public:
    ZoomControl(wxWindow& parent, real& zoom);
    void updateZoom();
  private:
    wxSlider& slider;
    real& zoom;
  };
}

#endif // !_FRACTALISM_ZOOM_CONTROL_HPP_
