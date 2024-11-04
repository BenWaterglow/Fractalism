#ifndef _SETTINGS_HPP_
#define _SETTINGS_HPP_

#include "CLTypes.hpp"
#include "Options.hpp"
#include "Utils.hpp"

template<class T>
concept SettingsChangeHandler =
  requires(T& t, cltypes::Number& v) { t.parameterChanged(v); } && 
  requires(T& t, int& v) { t.resolutionChanged(v); } &&
  requires(T& t, options::NumberSystem& v) { t.numberSystemChanged(v); } &&
  requires(T& t, options::RenderMode& v) { t.renderModeChanged(v); } &&
  requires(T& t, options::Dimensions& v) { t.renderDimensionsChanged(v); } &&
  requires(T& t, int& v) { t.iterationsPerFrameChanged(v); } &&
  requires(T& t, double& v) { t.iterationModifierChanged(v); } &&
  requires(T& t, cltypes::Viewspace& v) { t.phaseViewChanged(v); } &&
  requires(T& t, cltypes::Viewspace& v) { t.dynamicalViewChanged(v); };

template<class T> requires SettingsChangeHandler<T>
struct Settings {
private:
  template<typename V, auto callback>
  using Value = utils::ChangeAware<T, V, [](T& handler, V& value) { (handler.*callback)(value); }>;
public:
  Settings(T& handler) :
    parameter(handler),
    resolution(handler),
    numberSystem(handler),
    renderMode(handler),
    renderDimensions(handler),
    iterationsPerFrame(handler),
    iterationModifier(handler),
    phaseView(handler),
    dynamicalView(handler) {}
  Value<cltypes::Number, &T::parameterChanged> parameter;
  Value<int, &T::resolutionChanged> resolution;
  Value<options::NumberSystem, &T::numberSystemChanged> numberSystem;
  Value<options::RenderMode, &T::renderModeChanged> renderMode;
  Value<options::Dimensions, &T::renderDimensionsChanged> renderDimensions;
  Value<int, &T::iterationsPerFrameChanged> iterationsPerFrame;
  Value<double, &T::iterationModifierChanged> iterationModifier;
  Value<cltypes::Viewspace, &T::phaseViewChanged> phaseView;
  Value<cltypes::Viewspace, &T::dynamicalViewChanged> dynamicalView;
};

#endif