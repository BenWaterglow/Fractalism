#ifndef _FRACTALISM_EVENTS_HPP_
#define _FRACTALISM_EVENTS_HPP_

#include <Fractalism/UI/WxIncludeHelper.hpp>

#include <Fractalism/Options.hpp>

namespace fractalism {
  class StateChangeEvent : public wxEvent {
  public:
    StateChangeEvent(wxEventType eventType, int winId);
    virtual wxEvent* Clone() const;
    static void fireEvent(wxEventType eventType, wxWindow* window);

    template<options::Space space>
    static const wxEventTypeTag<StateChangeEvent>& viewChanged;
  };

  wxDECLARE_EVENT(EVT_ParameterChanged, StateChangeEvent);
  wxDECLARE_EVENT(EVT_PhaseViewChanged, StateChangeEvent);
  wxDECLARE_EVENT(EVT_DynamicalViewChanged, StateChangeEvent);
}
#endif