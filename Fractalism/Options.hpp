#ifndef _FRACTALISM_OPTIONS_HPP_
#define _FRACTALISM_OPTIONS_HPP_

#include <string>

#include <Fractalism/Exceptions.hpp>

namespace fractalism::options {
  enum class Dimensions : unsigned char {
    two,
    three
  };
  enum class RenderMode : unsigned char {
    escape,
    translated
  };
  inline constexpr const std::string name(const RenderMode renderMode) {
    switch (renderMode) {
    case RenderMode::escape: return "escape";
    case RenderMode::translated: return "translated";
    default: throw AssertionError("invalid render mode");
    }
  }
  enum class Space : unsigned char {
    phase = 1 << 0,
    dynamical = 1 << 1,
    both = phase | dynamical
  };
  inline constexpr const std::string name(const Space space) {
    switch (space) {
    case Space::phase: return "phase";
    case Space::dynamical: return "dynamical";
    default: throw AssertionError("invalid space");
    }
  }
  enum class NumberSystem : unsigned char {
    complex,
    bicomplex,
    quaternion
  };
  inline constexpr const std::string name(const NumberSystem numberSystem) {
    switch (numberSystem) {
    case NumberSystem::complex: return "complex";
    case NumberSystem::bicomplex: return "bicomplex";
    case NumberSystem::quaternion: return "quaternion";
    default: throw AssertionError("invalid number system");
    }
  }
  inline constexpr size_t elementCount(const NumberSystem numberSystem) {
    switch (numberSystem) {
    case NumberSystem::complex: return 2;
    case NumberSystem::bicomplex: [[fallthrough]];
    case NumberSystem::quaternion: return 4;
    default: throw AssertionError("Invalid number system");
    }
  }

  inline constexpr std::string kernelName(Space space, RenderMode renderMode, NumberSystem numberSystem) {
    return options::name(space) + "_" + options::name(renderMode) + "_" + options::name(numberSystem);
  }
}
#endif