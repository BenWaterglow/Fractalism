#ifndef _FRACTALISM_RENDER_SETTINGS_TOOL_BAR_
#define _FRACTALISM_RENDER_SETTINGS_TOOL_BAR_
#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/auibar.h>

namespace fractalism::ui::controls {

/**
 * @class RenderSettingsToolBar
 * @brief A toolbar for controlling render settings.
 */
class RenderSettingsToolBar : public wxAuiToolBar {
public:
  /**
   * @brief Constructs a RenderSettingsToolBar.
   * @param parent The parent window.
   */
  RenderSettingsToolBar(wxWindow& parent);

  /**
   * @brief Updates the render dimensions.
   */
  void updateRenderDimensions();

  /**
   * @brief Updates the resolution.
   */
  void updateResolution();

private:
  wxRadioBox& dimensions; ///< Radio box for selecting render dimensions.
  wxSlider& resolution;   ///< Slider for adjusting the resolution.
};
} // namespace fractalism::ui::controls

#endif
