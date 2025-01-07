#include <Fractalism/UI/Controls/ParameterToolBar.hpp>

#include <Fractalism/App.hpp>
#include <Fractalism/Settings.hpp>
#include <Fractalism/Events.hpp>

namespace fractalism::ui::controls {
  ParameterToolBar::ParameterToolBar(wxWindow& parent) :
        wxAuiToolBar(&parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_VERTICAL),
        control(*new HypercomplexNumberControl(*this, "Parameter", App::get<Settings>().parameter)) {
    AddControl(&control, "Parameter");

    control.Bind(events::NumberChanged::tag, [this](const events::NumberChanged::eventType& event) {
      events::ParameterChanged::fire(this, event.getValue());
    });
    Realize();
  }

  void ParameterToolBar::updateParameter()  {
    control.updateNumber();
  }
}