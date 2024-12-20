#include <Fractalism/Events.hpp>

namespace fractalism {
  wxDEFINE_EVENT(EVT_ViewChanged, StateChangeEvent);
  wxDEFINE_EVENT(EVT_ViewMappingChanged, StateChangeEvent);
  wxDEFINE_EVENT(EVT_ParameterChanged, StateChangeEvent);
  wxDEFINE_EVENT(EVT_PhaseViewChanged, StateChangeEvent);
  wxDEFINE_EVENT(EVT_DynamicalViewChanged, StateChangeEvent);

  template<>
  const wxEventTypeTag<StateChangeEvent>& StateChangeEvent::viewChanged<options::Space::phase> = EVT_PhaseViewChanged;

  template<>
  const wxEventTypeTag<StateChangeEvent>& StateChangeEvent::viewChanged<options::Space::dynamical> = EVT_DynamicalViewChanged;

  StateChangeEvent::StateChangeEvent(wxEventType eventType, int winId) :
    wxEvent(winId, eventType) {
    ResumePropagation(wxEVENT_PROPAGATE_MAX);
  }

  wxEvent* StateChangeEvent::Clone() const {
    return new StateChangeEvent(*this);
  }

  void StateChangeEvent::fireEvent(wxEventType eventType, wxWindow* window) {
    StateChangeEvent evt(eventType, window->GetId());
    evt.SetEventObject(window);
    window->ProcessWindowEvent(evt);
  }
}