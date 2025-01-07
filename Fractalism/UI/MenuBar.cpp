#include <Fractalism/UI/MenuBar.hpp>

#include <Fractalism/App.hpp>

namespace fractalism::ui {
  namespace {
    enum {
      ReloadShaders = wxID_HIGHEST + 1
    };
  }
  MenuBar::MenuBar() {
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ReloadShaders, "&Reload Shaders\tCtrl-R", "Reload the active shaders from disk");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    Append(menuFile, "&File");

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    Append(menuHelp, "&Help");

    Bind(wxEVT_MENU, [](wxCommandEvent&) {
      App::reloadShaders();
    }, ReloadShaders);
    Bind(wxEVT_MENU, [](wxCommandEvent&) {
      wxMessageBox("A unique application for fractal rendering and exploration.", "About Fractalism", wxOK | wxICON_INFORMATION);
    }, wxID_ABOUT);
    Bind(wxEVT_MENU, [](wxCommandEvent&) {
      App::get<wxFrame>().Close(true);
    }, wxID_EXIT);
  }
}