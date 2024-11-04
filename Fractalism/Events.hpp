#ifndef _EVENTS_HPP_
#define _EVENTS_HPP_

#include "WxIncludeHelper.hpp"


class StateChangeEvent : public wxEvent {
public:
  StateChangeEvent(wxEventType eventType, int winId);
  virtual wxEvent *Clone() const;
  static void fireEvent(wxEventType eventType, wxWindow *window);
};

wxDECLARE_EVENT(EVT_ParameterChanged, StateChangeEvent);
wxDECLARE_EVENT(EVT_PhaseViewChanged, StateChangeEvent);
wxDECLARE_EVENT(EVT_DynamicalViewChanged, StateChangeEvent);

#endif