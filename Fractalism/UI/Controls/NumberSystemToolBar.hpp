#ifndef _FRACTALISM_NUMBER_SYSTEM_TOOL_BAR_
#define _FRACTALISM_NUMBER_SYSTEM_TOOL_BAR_
#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/auibar.h>

namespace fractalism::ui::controls {

/**
 * @class NumberSystemToolBar
 * @brief A toolbar for selecting the number system.
 */
class NumberSystemToolBar : public wxAuiToolBar {
public:
  /**
   * @brief Constructs a NumberSystemToolBar.
   * @param parent The parent window.
   */
  NumberSystemToolBar(wxWindow& parent);

  /**
   * @brief Updates the number system selection.
   */
  void updateNumberSystem();

private:
  wxRadioBox& radioBox; ///< Radio box for selecting the number system.
};
} // namespace fractalism::ui::controls

#endif
