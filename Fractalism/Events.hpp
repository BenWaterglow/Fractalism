#ifndef _FRACTALISM_EVENTS_HPP_
#define _FRACTALISM_EVENTS_HPP_

#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/UI/UICommon.hpp>
#include <type_traits>

namespace fractalism::events {

/**
 * @class ValueChangeEvent
 * @brief Represents an event that indicates a change in value.
 * @tparam T The type of the value.
 */
template <typename T> class ValueChangeEvent : public wxEvent {
public:
  using ValueType =
      std::conditional_t<std::is_lvalue_reference_v<T>, T,
                         std::add_lvalue_reference_t<std::add_const_t<T>>>;

  /**
   * @brief Constructs a ValueChangeEvent.
   * @param window The window that generated the event.
   * @param eventType The type of the event.
   * @param value The new value.
   */
  ValueChangeEvent(wxWindow& window, wxEventType eventType, ValueType value) :
        wxEvent(window.GetId(), eventType), value(value) {
    SetEventObject(&window);
    ResumePropagation(wxEVENT_PROPAGATE_MAX);
  }

  /**
   * @brief Constructs a ValueChangeEvent.
   * @param window The window that generated the event.
   * @param eventType The type of the event.
   * @param value The new value.
   */
  ValueChangeEvent(wxWindow* window, wxEventType eventType, ValueType value) :
        ValueChangeEvent(*window, eventType, value) {}

  /**
   * @brief Clones the event.
   * @return A pointer to the cloned event.
   */
  virtual wxEvent *Clone() const override {
    return new ValueChangeEvent(*this);
  }

  /**
   * @brief Implicit conversion to the value type.
   * @return The new value.
   */
  inline operator ValueType() const { return value; }

  /**
   * @brief Gets the new value.
   * @return The new value.
   */
  inline ValueType getValue() const { return value; }

private:
  ValueType value; ///< The new value.
};

/**
 * @class StateChangeEvent
 * @brief Represents an event that indicates a change in state.
 * @tparam T The type of the state.
 */
template <typename T> using StateChangeEvent = ValueChangeEvent<T &>;

/**
 * @concept EventTag
 * @brief Concept for event tags.
 * @tparam T The type of the event tag.
 */
template <typename T>
concept EventTag = std::convertible_to<decltype(T::tag),
                                       wxEventTypeTag<typename T::eventType>>;

/**
 * @struct Event
 * @brief Represents an event with a specific tag and arguments.
 * @tparam Tag The tag of the event.
 * @tparam Args The types of the arguments for the event.
 */
template<EventTag Tag, typename... Args>
struct Event {
  using eventType = Tag::eventType;
  static constexpr const wxEventTypeTag<typename Tag::eventType>& tag = Tag::tag;

  /**
   * @brief Fires the event.
   * @param window The window that generated the event.
   * @param value The new value.
   * @param args Additional arguments for the event.
   * @return True if the event was processed, false otherwise.
   */
  static inline bool fire(
      wxWindow* window,
      const eventType::ValueType& value,
      const Args&... args) {
    return fire(*window, value, args...);
  }

  /**
   * @brief Fires the event.
   * @param window The window that generated the event.
   * @param value The new value.
   * @param args Additional arguments for the event.
   * @return True if the event was processed, false otherwise.
   */
  static inline bool fire(
      wxWindow& window,
      const eventType::ValueType& value,
      const Args&... args) {
    eventType event(window, Tag::tag, value, args...);
    return window.ProcessWindowEvent(event);
  }
};

/**
 * @brief Macro to declare an event.
 * @param name The name of the event.
 * @param evtType The type of the event.
 * @param ... Additional type arguments for the event.
 */
#define DECLARE_EVENT(name, evtType, ...)                                      \
  namespace tags {                                                             \
  struct name##Tag {                                                           \
    using eventType = evtType;                                                 \
    static const wxEventTypeTag<eventType> tag;                                \
  };                                                                           \
  }                                                                            \
  struct name : public Event<tags::name##Tag##__VA_OPT__(, )__VA_ARGS__> {}

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
} // namespace fractalism::events

#endif
