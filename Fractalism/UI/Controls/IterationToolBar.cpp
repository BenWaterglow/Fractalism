#include <Fractalism/UI/Controls/IterationToolBar.hpp>

#include <Fractalism/Events.hpp>
#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism::ui::controls {

  IterationToolBar::IterationToolBar(wxWindow& parent, ViewWindowSettings& settings) :
        wxAuiToolBar(&parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_TEXT | wxAUI_TB_VERTICAL),
        settings(settings),
        iterationModifier(*new wxSlider(this, wxID_ANY, settings.iterationModifier / 5, 1, 100)),
        iterationsPerFrame(*new wxSlider(this, wxID_ANY, settings.iterationsPerFrame, 1, 1000)) {
    AddControl(&iterationModifier, "Iteration Modifier");
    AddControl(&iterationsPerFrame, "Iterations Per Frame");

    iterationModifier.Bind(wxEVT_SLIDER, [this, &iterationModifier = settings.iterationModifier](wxCommandEvent& evt) {
      iterationModifier = evt.GetInt() * 5.0;
      events::IterationModifierChanged::fire(this, iterationModifier);
    });

    iterationsPerFrame.Bind(wxEVT_SLIDER, [this, &iterationsPerFrame = settings.iterationsPerFrame](wxCommandEvent& evt) {
      iterationsPerFrame = evt.GetInt();
      events::IterationsPerFrameChanged::fire(this, iterationsPerFrame);
    });
    Realize();
  }

  void IterationToolBar::updateIterationModifier() {
    iterationModifier.SetValue(settings.iterationModifier / 5);
  }

  void IterationToolBar::updateIterationsPerFrame() {
    iterationsPerFrame.SetValue(settings.iterationsPerFrame);
  }
}