#include <Fractalism/Events.hpp>

namespace fractalism::events {

  #define DEFINE_EVENT(name, ...) const wxEventTypeTag<tags::name##Tag::eventType> tags::name##Tag::tag(wxNewEventType())

  DEFINE_EVENT(NumberChanged);
  DEFINE_EVENT(ViewCenterChanged);
  DEFINE_EVENT(ZoomChanged);
  DEFINE_EVENT(ViewMappingChanged);
  DEFINE_EVENT(ParameterChanged);
  DEFINE_EVENT(IterationModifierChanged);
  DEFINE_EVENT(IterationsPerFrameChanged);
  DEFINE_EVENT(NumberSystemChanged);
  DEFINE_EVENT(RenderDimensionsChanged);
  DEFINE_EVENT(ResolutionChanged);
  DEFINE_EVENT(CoordinatesChanged);
  #undef DEFINE_EVENT
}