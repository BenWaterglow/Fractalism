#ifndef _FRACTALISM_PARAMETER_TOOL_BAR_HPP_
#define _FRACTALISM_PARAMETER_TOOL_BAR_HPP_
#include <Fractalism/UI/UICommon.hpp>
#include <wx/aui/auibar.h>

#include <Fractalism/gpu/Types.hpp>
#include <Fractalism/UI/Controls/HypercomplexNumberControl.hpp>

namespace fractalism::ui::controls {
  class ParameterToolBar : public wxAuiToolBar {
  public:
    ParameterToolBar(wxWindow& parent);
    void updateParameter();
  private:
    HypercomplexNumberControl& control;
  };
}

#endif