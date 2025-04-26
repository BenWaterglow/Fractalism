#ifndef _FRACTALISM_VIEW_WINDOW_SETTINS_HPP_
#define _FRACTALISM_VIEW_WINDOW_SETTINS_HPP_

#include <algorithm>
#include <limits>

#include <Fractalism/GPU/OpenGL/ArcballCamera.hpp>
#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/Options.hpp>

namespace fractalism {

/**
 * @struct ViewWindowSettings
 * @brief Manages settings for a view window, including zoom, iterations, and
 * rendering modes.
 */
struct ViewWindowSettings {
  static constexpr double zoom1x = 1.5;     ///< Default zoom level.
  static constexpr double minZoom = 0.1;    ///< Minimum zoom level.
  static constexpr double maxZoom = 1.0e16; ///< Maximum zoom level.

  static constexpr double minIterations = 1.0; ///< Minimum number of iterations.
  static constexpr double maxIterations = std::numeric_limits<cl_uint>::max(); ///< Maximum number of iterations.

  /**
   * @brief Constructs a ViewWindowSettings object with the specified space and
   * render mode.
   * @param space The space setting.
   * @param renderMode The render mode setting.
   */
  ViewWindowSettings(options::Space space, options::RenderMode renderMode);

  /**
   * @brief Gets the number of iterations per frame based on the render mode.
   * @return The number of iterations per frame.
   */
  inline cl_uint getIterationsPerFrame() const {
    switch (renderMode) {
    case options::RenderMode::escape:
      return iterationsPerFrame;
    case options::RenderMode::translated:
      return 1;
    default:
      throw AssertionError("Invalid render mode");
    }
  }

  /**
   * @brief Gets the maximum number of iterations based on the render mode and
   * zoom level.
   * @return The maximum number of iterations.
   */
  inline cl_uint getMaxIterations() const {
    switch (renderMode) {
    case options::RenderMode::escape:
      return static_cast<cl_uint>(std::clamp(
          iterationModifier * pow(2.0, log10((view.zoom))),
          minIterations,
          maxIterations));
    case options::RenderMode::translated:
      return maxIterations;
    default:
      throw AssertionError("Invalid render mode");
    }
  }

  gpu::types::Viewspace view;        ///< The viewspace settings.
  gpu::opengl::ArcballCamera camera; ///< The arcball camera settings.
  real iterationModifier;            ///< The iteration modifier.
  cl_uint iterationsPerFrame;        ///< The number of iterations per frame.
  options::RenderMode renderMode;    ///< The render mode.
  options::Space space;              ///< The space setting.
};
} // namespace fractalism

#endif
