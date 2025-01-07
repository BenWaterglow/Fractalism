#ifndef _FRACTALISM_UI_HPP_
#define _FRACTALISM_UI_HPP_

#include <vector>
#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/framemanager.h>

#include <Fractalism/Events.hpp>
#include <Fractalism/UI/ViewWindow.hpp>

namespace fractalism::ui {
  class UI : public wxFrame {
  public:
    UI();
  private:
    void updateFps();
    wxAuiManager frameManager;
    wxLongLong lastRenderMillis;
    std::vector<ViewWindow*> viewWindows;
    double fps;
  };
}
#endif