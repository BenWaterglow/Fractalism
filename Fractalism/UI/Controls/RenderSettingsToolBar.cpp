#include <Fractalism/UI/Controls/RenderSettingsToolBar.hpp>

#include <Fractalism/App.hpp>
#include <Fractalism/Events.hpp>
#include <Fractalism/Settings.hpp>

namespace fractalism::ui::controls {
  
  namespace {
    wxString dimensionLabels[] = { "2D", "3D" };
  }

  RenderSettingsToolBar::RenderSettingsToolBar(wxWindow& parent) :
        wxAuiToolBar(&parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_TEXT | wxAUI_TB_VERTICAL),
        dimensions(*new wxRadioBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, dimensionLabels)),
        resolution(*new wxSlider(this, wxID_ANY, App::get<Settings>().resolution[0] / 64, 1, 20)) {
    AddControl(&dimensions, "Dimensions");
    AddControl(&resolution, "Resolution");
    dimensions.Bind(wxEVT_RADIOBOX, [this](wxCommandEvent& evt) {
      events::RenderDimensionsChanged::fire(this, App::get<Settings>().setRenderDimensions(utils::fromUnderlyingType<options::Dimensions>(evt.GetInt())));
    });
    resolution.Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
      events::ResolutionChanged::fire(this, App::get<Settings>().setResolution(static_cast<cl::size_type>(evt.GetInt()) * 64));
    });
    updateRenderDimensions();
    updateResolution();
    Realize();
  }

  void RenderSettingsToolBar::updateRenderDimensions() {
    dimensions.SetSelection(utils::toUnderlyingType<options::Dimensions>(App::get<Settings>().renderDimensions));
  }

  void RenderSettingsToolBar::updateResolution() {
    resolution.SetValue(App::get<Settings>().resolution[0] / 64);
  }
}