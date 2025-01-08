#ifndef _FRACTALISM_ITERATION_TOOL_BAR_HPP
#define _FRACTALISM_ITERATION_TOOL_BAR_HPP
#include <Fractalism/UI/UICommon.hpp>
#include <Fractalism/ViewWindowSettings.hpp>
#include <wx/aui/auibar.h>

namespace fractalism::ui::controls {

/**
 * @class IterationToolBar
 * @brief A toolbar for controlling iteration settings.
 */
class IterationToolBar : public wxAuiToolBar {
public:
  /**
   * @brief Constructs an IterationToolBar.
   * @param parent The parent window.
   * @param settings The view window settings.
   */
  IterationToolBar(wxWindow& parent, ViewWindowSettings& settings);

  /**
   * @brief Updates the iteration modifier.
   */
  void updateIterationModifier();

  /**
   * @brief Updates the iterations per frame.
   */
  void updateIterationsPerFrame();

private:
  ViewWindowSettings& settings; ///< The view window settings.
  wxSlider& iterationModifier;  ///< Slider for the iteration modifier.
  wxSlider& iterationsPerFrame; ///< Slider for the iterations per frame.
};
} // namespace fractalism::ui::controls

#endif
