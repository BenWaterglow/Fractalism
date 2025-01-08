#ifndef _FRACTALISM_ZOOM_CONTROL_HPP_
#define _FRACTALISM_ZOOM_CONTROL_HPP_

#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/UI/UICommon.hpp>

namespace fractalism::ui::controls {

/**
 * @class ZoomControl
 * @brief A control for adjusting the zoom level.
 */
class ZoomControl : public wxControl {
public:
  /**
   * @brief Constructs a ZoomControl.
   * @param parent The parent window.
   * @param zoom The zoom level to control.
   */
  ZoomControl(wxWindow& parent, real& zoom);

  /**
   * @brief Updates the zoom level.
   */
  void updateZoom();

private:
  wxSlider& slider; ///< Slider for adjusting the zoom level.
  real& zoom;       ///< The zoom level to control.
};
} // namespace fractalism::ui::controls

#endif // !_FRACTALISM_ZOOM_CONTROL_HPP_
