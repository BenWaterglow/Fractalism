#include <Fractalism/UI/Controls/NumberSystemToolBar.hpp>

#include <Fractalism/App.hpp>
#include <Fractalism/Settings.hpp>
#include <Fractalism/Events.hpp>

namespace fractalism::ui::controls {
  namespace {
    wxString labels[] = { "Complex", "Bicomplex", "Quaternion" };
  }

  NumberSystemToolBar::NumberSystemToolBar(wxWindow& parent) :
        wxAuiToolBar(&parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_VERTICAL),
        radioBox(*new wxRadioBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, labels, 0, wxRA_SPECIFY_ROWS)) {
    AddControl(&radioBox, "Number System");
    radioBox.Bind(wxEVT_RADIOBOX, [this](wxCommandEvent& evt) {
      options::NumberSystem& numberSystem = App::get<Settings>().numberSystem;
      numberSystem = utils::fromUnderlyingType<options::NumberSystem>(evt.GetInt());
      events::NumberSystemChanged::fire(this, numberSystem);
    });
    updateNumberSystem();
    Realize();
  }

  void NumberSystemToolBar::updateNumberSystem() {
    radioBox.SetSelection(utils::toUnderlyingType<options::NumberSystem>(App::get<Settings>().numberSystem));
  }
}