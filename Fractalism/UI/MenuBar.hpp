#ifndef _FRACTALISM_MENU_BAR_HPP_
#define _FRACTALISM_MENU_BAR_HPP_
#include <Fractalism/UI/UICommon.hpp>

namespace fractalism::ui {

/**
 * @class MenuBar
 * @brief The menu bar for the Fractalism application.
 */
class MenuBar : public wxMenuBar {
public:
  /**
   * @brief Constructs a MenuBar.
   */
  MenuBar();
};
} // namespace fractalism::ui

#endif
