#include <Fractalism/UI/MenuBar.hpp>

#include <Fractalism/UI/IDs.hpp>
#include <Fractalism/App.hpp>

namespace fractalism {
  namespace ui {

    fractalism::ui::MenuBar::MenuBar() {
      wxMenu* menuFile = new wxMenu;
      menuFile->Append(ids::ReloadShaders, "&Reload Shaders\tCtrl-R", "Reload the active shaders from disk");
      menuFile->AppendSeparator();
      menuFile->Append(wxID_EXIT);
      Append(menuFile, "&File");

      wxMenu* menuHelp = new wxMenu;
      menuHelp->Append(wxID_ABOUT);
      Append(menuHelp, "&Help");

      Bind(wxEVT_MENU, [](wxCommandEvent&) {
        App::reloadShaders();
      }, ids::ReloadShaders);
      Bind(wxEVT_MENU, [](wxCommandEvent&) {
        wxMessageBox("A unique application for fractal rendering and exploration.", "About Fractalism", wxOK | wxICON_INFORMATION);
      }, wxID_ABOUT);
      Bind(wxEVT_MENU, [](wxCommandEvent&) {
        App::get<UI>().Close(true);
      }, wxID_EXIT);
    }
  }
}