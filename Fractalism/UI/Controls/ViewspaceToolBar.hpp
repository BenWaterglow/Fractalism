#ifndef _FRACTALISM_VIEWSPACE_CONTROL_HPP_
#define _FRACTALISM_VIEWSPACE_CONTROL_HPP_

#include <Fractalism/Events.hpp>
#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/UI/Controls/HypercomplexNumberControl.hpp>
#include <Fractalism/UI/Controls/ViewMappingControl.hpp>
#include <Fractalism/UI/Controls/ZoomControl.hpp>
#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/auibar.h>

namespace fractalism::ui::controls {

/**
 * @class ViewspaceToolBar
 * @brief A toolbar for controlling the viewspace settings.
 */
class ViewspaceToolBar : public wxAuiToolBar {
public:
  /**
   * @brief Constructs a ViewspaceToolBar.
   * @param parent The parent window.
   * @param viewspace The viewspace to control.
   */
  ViewspaceToolBar(wxWindow& parent, gpu::types::Viewspace& viewspace);

  /**
   * @brief Updates the center of the viewspace.
   */
  void updateCenter();

  /**
   * @brief Updates the view mapping.
   */
  void updateViewMapping();

  /**
   * @brief Updates the zoom level.
   */
  void updateZoom();

  /**
   * @brief Updates the number system.
   */
  void updateNumberSystem();

  /**
   * @brief Updates the render dimensions.
   */
  void updateRenderDimensions();

private:
  gpu::types::Viewspace& view;            ///< The viewspace to control.
  HypercomplexNumberControl& numberInput; ///< Control for inputting hypercomplex numbers.
  ViewMappingControl& viewMapping;        ///< Control for selecting view mappings.
  ZoomControl& zoom;                      ///< Control for adjusting the zoom level.
};
} // namespace fractalism::ui::controls

#endif // !_FRACTALISM_VIEWSPACE_CONTROL_HPP_
