#ifndef _FRACTALISM_OPTIONS_HPP_
#define _FRACTALISM_OPTIONS_HPP_

#include <Fractalism/Exceptions.hpp>
#include <string>

namespace fractalism::options {

/**
 * @enum Dimensions
 * @brief Represents the dimensions for rendering.
 */
enum class Dimensions : unsigned char {
  two,  ///< 2D rendering.
  three ///< 3D rendering.
};

/**
 * @enum RenderMode
 * @brief Represents the render modes.
 */
enum class RenderMode : unsigned char {
  escape,    ///< Escape time render mode.
  translated ///< Translated render mode.
};

/**
 * @brief Gets the name of the render mode.
 * @param renderMode The render mode.
 * @return The name of the render mode as a string.
 */
inline constexpr const std::string name(const RenderMode renderMode) {
  switch (renderMode) {
  case RenderMode::escape:
    return "escape";
  case RenderMode::translated:
    return "translated";
  default:
    throw AssertionError("invalid render mode");
  }
}

/**
 * @enum Space
 * @brief Represents the space settings.
 */
enum class Space : unsigned char {
  phase,          ///< Phase space.
  dynamical,      ///< Dynamical space.
};

/**
 * @brief Gets the name of the space.
 * @param space The space.
 * @return The name of the space as a string.
 */
inline constexpr const std::string name(const Space space) {
  switch (space) {
  case Space::phase:
    return "phase";
  case Space::dynamical:
    return "dynamical";
  default:
    throw AssertionError("invalid space");
  }
}

/**
 * @enum NumberSystem
 * @brief Represents the number systems.
 */
enum class NumberSystem : unsigned char {
  complex,   ///< Complex number system.
  bicomplex, ///< Bicomplex number system.
  quaternion ///< Quaternion number system.
};

/**
 * @brief Gets the name of the number system.
 * @param numberSystem The number system.
 * @return The name of the number system as a string.
 */
inline constexpr const std::string name(const NumberSystem numberSystem) {
  switch (numberSystem) {
  case NumberSystem::complex:
    return "complex";
  case NumberSystem::bicomplex:
    return "bicomplex";
  case NumberSystem::quaternion:
    return "quaternion";
  default:
    throw AssertionError("invalid number system");
  }
}

/**
 * @brief Gets the number of elements in the number system.
 * @param numberSystem The number system.
 * @return The number of elements in the number system.
 */
inline constexpr size_t elementCount(const NumberSystem numberSystem) {
  switch (numberSystem) {
  case NumberSystem::complex:
    return 2;
  case NumberSystem::bicomplex:
    [[fallthrough]];
  case NumberSystem::quaternion:
    return 4;
  default:
    throw AssertionError("Invalid number system");
  }
}

/**
 * @brief Constructs the kernel name based on space, render mode, and number
 * system.
 * @param space The space setting.
 * @param renderMode The render mode setting.
 * @param numberSystem The number system setting.
 * @return The kernel name as a string.
 */
inline constexpr std::string kernelName(Space space, RenderMode renderMode, NumberSystem numberSystem) {
  return options::name(space) + "_" + options::name(renderMode) + "_" + options::name(numberSystem);
}
} // namespace fractalism::options

#endif
