#ifndef _FRACTALISM_UI_HPP_
#define _FRACTALISM_UI_HPP_

#include <Fractalism/UI/UICommon.hpp>
#include <vector>
#include <wx/aui/framemanager.h>

#include <Fractalism/Events.hpp>
#include <Fractalism/UI/ViewWindow.hpp>

namespace fractalism::ui {

/**
 * @class UI
 * @brief Main user interface class for the Fractalism application.
 */
class UI : public wxFrame {
public:
  /**
   * @brief Constructs the UI frame and initializes the interface.
   */
  UI();

private:
  /**
   * @brief Updates the frames per second (FPS) display in the status bar.
   */
  void updateFps();

  wxAuiManager frameManager;            ///< Manager for the frame layout.
  wxLongLong lastRenderMillis;          ///< Timestamp of the last render in milliseconds.
  std::vector<ViewWindow*> viewWindows; ///< List of view windows in the UI.
  double fps;                           ///< Current frames per second (FPS) value.
};
} // namespace fractalism::ui

#endif
