#ifndef _FRACTALISM_ITERATION_TOOL_BAR_HPP
#define _FRACTALISM_ITERATION_TOOL_BAR_HPP
#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/auibar.h>

#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism::ui::controls {
  class IterationToolBar : public wxAuiToolBar {
  public:
    IterationToolBar(wxWindow& parent, ViewWindowSettings& settings);
    void updateIterationModifier();
    void updateIterationsPerFrame();
  private:
    ViewWindowSettings& settings;
    wxSlider& iterationModifier;
    wxSlider& iterationsPerFrame;
  };
}

#endif