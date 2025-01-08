#ifndef _FRACTALISM_VIEW_MAPPING_SELECTOR_HPP_
#define _FRACTALISM_VIEW_MAPPING_SELECTOR_HPP_

#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/UI/UICommon.hpp>

namespace fractalism::ui::controls {

/**
 * @class ViewMappingControl
 * @brief A control for selecting view mappings.
 */
class ViewMappingControl : public wxControl {
public:
  /**
   * @brief Constructs a ViewMappingControl.
   * @param parent The parent window.
   * @param label The label for the control.
   * @param mapping The view mapping to control.
   */
  ViewMappingControl(
      wxWindow& parent,
      wxString label,
      gpu::types::ViewMapping& mapping);

  /**
   * @brief Updates the view mapping.
   */
  void updateViewMapping();

  /**
   * @brief Updates the number system.
   */
  void updateNumberSystem();

  /**
   * @brief Updates the render dimensions.
   */
  void updateRenderDimensions();

private:
  /**
   * @class Row
   * @brief A row in the ViewMappingControl for a specific view mapping element.
   * @tparam index The index of the view mapping element.
   */
  template <cl_char gpu::types::ViewMapping::*index>
  class Row : public wxBoxSizer {
  public:
    /**
     * @brief Constructs a Row.
     * @param parent The parent control.
     * @param label The label for the row.
     * @param mapping The view mapping to control.
     */
    Row(
        ViewMappingControl& parent,
        wxString label,
        gpu::types::ViewMapping& mapping);

    /**
     * @brief Updates the number system for the row.
     */
    void updateNumberSystem();

    /**
     * @brief Updates the view mapping element.
     */
    void updateViewMappingElement();

  private:
    wxCheckBox& inverted;             ///< Check box for inverting the view mapping element.
    wxChoice& choice;                 ///< Choice control for selecting the view mapping element.
    gpu::types::ViewMapping& mapping; ///< The view mapping to control.

    /**
     * @brief Handles the event when the invert check box is toggled.
     * @param evt The event.
     */
    void onInvert(wxCommandEvent& evt);

    /**
     * @brief Handles the event when the mapping choice is changed.
     * @param evt The event.
     */
    void onMappingChanged(wxCommandEvent& evt);
  };

  Row<&gpu::types::ViewMapping::x>& x; ///< Row for the x view mapping element.
  Row<&gpu::types::ViewMapping::y>& y; ///< Row for the y view mapping element.
  Row<&gpu::types::ViewMapping::z>& z; ///< Row for the z view mapping element.
};
} // namespace fractalism::ui::controls

#endif
