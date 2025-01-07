#ifndef _FRACTALISM_EVENTS_HPP_
#define _FRACTALISM_EVENTS_HPP_

#include <type_traits>

#include <Fractalism/UI/UICommon.hpp>
#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/Options.hpp>

namespace fractalism::events {
  template<typename T>
  class ValueChangeEvent : public wxEvent {
  public:
    using ValueType = std::conditional_t<std::is_lvalue_reference_v<T>, T, std::add_lvalue_reference_t<std::add_const_t<T>>>;
    ValueChangeEvent(wxWindow& window, wxEventType eventType, ValueType value) :
          wxEvent(window.GetId(), eventType), value(value) {
      SetEventObject(&window);
      ResumePropagation(wxEVENT_PROPAGATE_MAX);
    }
    ValueChangeEvent(wxWindow* window, wxEventType eventType, ValueType value) :
          ValueChangeEvent(*window, eventType, value) {}
    virtual wxEvent* Clone() const override { return new ValueChangeEvent(*this); }
    inline operator ValueType() const { return value; }
    inline ValueType getValue() const { return value; }
  private:
    ValueType value;
  };

  template<typename T>
  using StateChangeEvent = ValueChangeEvent<T&>;

  template<typename T>
  concept EventTag = std::convertible_to<decltype(T::tag), wxEventTypeTag<typename T::eventType>>;

  template <EventTag Tag, typename... Args>
  struct Event {
    using eventType = Tag::eventType;
    static constexpr const wxEventTypeTag<typename Tag::eventType>& tag = Tag::tag;
    static inline bool fire(wxWindow* window, const eventType::ValueType& value, const Args&... args) { return fire(*window, value, args...); }
    static inline bool fire(wxWindow& window, const eventType::ValueType& value, const Args&... args) {
      eventType event(window, Tag::tag, value, args...);
      return window.ProcessWindowEvent(event);
    }
  };

  #define DECLARE_EVENT(name, evtType, ...) \
      namespace tags { struct name##Tag { using eventType = evtType; static const wxEventTypeTag<eventType> tag; }; } \
      struct name : public Event<tags::name##Tag##__VA_OPT__(,)__VA_ARGS__> {}

  DECLARE_EVENT(NumberChanged, StateChangeEvent<gpu::types::Number>);
  DECLARE_EVENT(ViewCenterChanged, StateChangeEvent<gpu::types::Number>);
  DECLARE_EVENT(ZoomChanged, StateChangeEvent<real>);
  DECLARE_EVENT(ViewMappingChanged, StateChangeEvent<gpu::types::ViewMapping>);
  DECLARE_EVENT(ParameterChanged, StateChangeEvent<gpu::types::Number>);
  DECLARE_EVENT(IterationModifierChanged, StateChangeEvent<real>);
  DECLARE_EVENT(IterationsPerFrameChanged, StateChangeEvent<cl_uint>);
  DECLARE_EVENT(NumberSystemChanged, StateChangeEvent<options::NumberSystem>);
  DECLARE_EVENT(RenderDimensionsChanged, StateChangeEvent<options::Dimensions>);
  DECLARE_EVENT(ResolutionChanged, StateChangeEvent<cl::NDRange>);
  DECLARE_EVENT(CoordinatesChanged, ValueChangeEvent<gpu::types::Coordinates>);

  #undef DECLARE_EVENT
}
#endif