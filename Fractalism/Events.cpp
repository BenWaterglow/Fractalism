#include "Events.hpp"

wxDEFINE_EVENT(EVT_ParameterChanged, StateChangeEvent);
wxDEFINE_EVENT(EVT_PhaseViewChanged, StateChangeEvent);
wxDEFINE_EVENT(EVT_DynamicalViewChanged, StateChangeEvent);

StateChangeEvent::StateChangeEvent(wxEventType eventType, int winId) :
    wxEvent(winId, eventType) {
  ResumePropagation(wxEVENT_PROPAGATE_MAX);
}

wxEvent *StateChangeEvent::Clone() const {
  return new StateChangeEvent(*this);
}

void StateChangeEvent::fireEvent(wxEventType eventType, wxWindow *window) {
  StateChangeEvent evt(eventType, window->GetId());
  evt.SetEventObject(window);
  window->ProcessWindowEvent(evt);
}