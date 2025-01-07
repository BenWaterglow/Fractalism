#ifndef _FRACTALISM_RENDER_SETTINGS_TOOL_BAR_
#define _FRACTALISM_RENDER_SETTINGS_TOOL_BAR_
#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/auibar.h>

namespace fractalism::ui::controls {
  class RenderSettingsToolBar : public wxAuiToolBar {
  public:
    RenderSettingsToolBar(wxWindow& parent);
    void updateRenderDimensions();
    void updateResolution();
  private:
    wxRadioBox& dimensions;
    wxSlider& resolution;
  };
}

#endif