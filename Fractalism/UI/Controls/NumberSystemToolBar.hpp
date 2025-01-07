#ifndef _FRACTALISM_NUMBER_SYSTEM_TOOL_BAR_
#define _FRACTALISM_NUMBER_SYSTEM_TOOL_BAR_
#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/auibar.h>

namespace fractalism::ui::controls {
  class NumberSystemToolBar : public wxAuiToolBar {
  public:
    NumberSystemToolBar(wxWindow& parent);
    void updateNumberSystem();
  private:
    wxRadioBox &radioBox;
  };
}

#endif