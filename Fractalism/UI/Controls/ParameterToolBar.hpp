#ifndef _FRACTALISM_PARAMETER_TOOL_BAR_HPP_
#define _FRACTALISM_PARAMETER_TOOL_BAR_HPP_
#include <Fractalism/UI/Controls/HypercomplexNumberControl.hpp>
#include <Fractalism/UI/UICommon.hpp>
#include <Fractalism/gpu/Types.hpp>
#include <wx/aui/auibar.h>

namespace fractalism::ui::controls {

/**
 * @class ParameterToolBar
 * @brief A toolbar for controlling parameters.
 */
class ParameterToolBar : public wxAuiToolBar {
public:
  /**
   * @brief Constructs a ParameterToolBar.
   * @param parent The parent window.
   */
  ParameterToolBar(wxWindow& parent);

  /**
   * @brief Updates the parameter control.
   */
  void updateParameter();

private:
  HypercomplexNumberControl& control; ///< Control for inputting hypercomplex numbers.
};
} // namespace fractalism::ui::controls

#endif
