#ifndef _FRACTALISM_VIEW_WINDOW_SETTINS_HPP_
#define _FRACTALISM_VIEW_WINDOW_SETTINS_HPP_

#include <limits>
#include <algorithm>

#include <Fractalism/Options.hpp>
#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/GPU/OpenGL/ArcballCamera.hpp>

namespace fractalism {
  struct ViewWindowSettings {
    static constexpr double zoom1x = 1.5;
    static constexpr double minZoom = 0.1;
    static constexpr double maxZoom = 1.0e16;

    static constexpr double minIterations = 1.0;
    static constexpr double maxIterations = std::numeric_limits<cl_uint>::max();

    ViewWindowSettings(options::Space space, options::RenderMode renderMode);

    inline cl_uint getIterationsPerFrame() const {
      switch(renderMode) {
      case options::RenderMode::escape: return iterationsPerFrame;
      case options::RenderMode::translated: return 1;
      default: throw AssertionError("Invalid render mode");
      }
    }
    inline cl_uint getMaxIterations() const {
      switch(renderMode) {
      case options::RenderMode::escape: return static_cast<cl_uint>(std::clamp(
            iterationModifier * pow(2.0, log10((zoom1x / view.zoom) / 2.0)),
            minIterations,
            maxIterations));
      case options::RenderMode::translated: return maxIterations;
      default: throw AssertionError("Invalid render mode");
      }
    }
    gpu::types::Viewspace view;
    gpu::opengl::ArcballCamera camera;
    real iterationModifier;
    cl_uint iterationsPerFrame;
    options::RenderMode renderMode;
    options::Space space;
  };
}

#endif