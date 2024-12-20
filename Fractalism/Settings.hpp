#ifndef _FRACTALISM_SETTINGS_HPP_
#define _FRACTALISM_SETTINGS_HPP_

#include <string>
#include <limits>
#include <algorithm>
#include <glm/glm.hpp>

#include <Fractalism/GPU/OpenCL/CLTypes.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/Utils.hpp>
#include <Fractalism/Proxy.hpp>

namespace fractalism {
  struct Settings {
  private:
    template<typename V, auto callback>
    using Value = proxy::CallbackProxy<V, callback>;
    static void parameterChanged();
    static void resolutionChanged();
    static void numberSystemChanged();
    static void renderModeChanged();
    static void renderDimensionsChanged();
    static void spaceChanged();
    static void iterationsPerFrameChanged();
    static void iterationModifierChanged();
    static void phaseViewChanged();
    static void dynamicalViewChanged();
    static void trackballChanged();
    cl_char phaseZMapping;
    cl_char dynamicalZMapping;
  public:
    static constexpr double zoom1x = 1.5;
    static constexpr double minZoom = 0.1;
    static constexpr double maxZoom = 10000000000000000.0;
    static constexpr double minIterations = 1.0;
    static constexpr double maxIterations = std::numeric_limits<cl_uint>::max();

    proxy::CallbackProxy<options::NumberSystem, &numberSystemChanged> numberSystem;
    proxy::CallbackProxy<options::Dimensions, &renderDimensionsChanged> renderDimensions;
    proxy::CallbackProxy<cl_uint, &resolutionChanged> resolution;

    Value<options::RenderMode, &renderModeChanged> renderMode;
    Value<options::Space, &spaceChanged> space;
    Value<cl_uint, &iterationsPerFrameChanged> iterationsPerFrame;
    Value<double, &iterationModifierChanged> iterationModifier;
    Value<glm::dvec3, &trackballChanged> trackball;
    Value<gpu::opencl::cltypes::Number, &parameterChanged> parameter;
    Value<gpu::opencl::cltypes::Viewspace, &phaseViewChanged> phaseView;
    Value<gpu::opencl::cltypes::Viewspace, &dynamicalViewChanged> dynamicalView;

    Settings();

    inline bool phaseEnabled() const {
      return utils::enabled<options::Space::phase>(space);
    }

    inline bool dynamicalEnabled() const {
      return utils::enabled<options::Space::dynamical>(space);
    }

    inline cl_uint getIterationsPerFrame() const {
      switch (renderMode) {
      case options::RenderMode::escape: return iterationsPerFrame;
      case options::RenderMode::translated: return 1;
      default: throw std::invalid_argument("Invalid render mode.");
      }
    }

    inline cl_uint getMaxPhaseIterations() const {
      return getMaxIterations(phaseView.unwrapValue<real>());
    }

    inline cl_uint getMaxDynamicalIterations() const {
      return getMaxIterations(dynamicalView.unwrapValue<real>());
    }

    inline cl_uint getMaxIterations(real zoom) const {
      switch (renderMode) {
      case options::RenderMode::escape: {
        return static_cast<cl_uint>(std::clamp(
          iterationModifier * pow(2.0, log10((zoom1x / zoom) / 2.0)),
          minIterations,
          maxIterations));
      }
      case options::RenderMode::translated: return maxIterations;
      default: throw std::invalid_argument("Invalid render mode.");
      }
    }

    inline cl::NDRange getRenderVolumeSize() const {
      switch (renderDimensions) {
      case options::Dimensions::two: return cl::NDRange(resolution, resolution);
      case options::Dimensions::three: return cl::NDRange(resolution, resolution, resolution);
      default: throw std::invalid_argument("Invalid render dimensions");
      }
    }

    inline std::string getPhaseKernelName() const {
      return "phase_" + getRenderModeName() + "_" + getNumberSystemName();
    }

    inline std::string getDynamicalKernelName() const {
      return "dynamical_" + getRenderModeName() + "_" + getNumberSystemName();
    }

    inline std::string getRenderModeName() const {
      switch (renderMode) {
      case options::RenderMode::escape: return "escape";
      case options::RenderMode::translated: return "translated";
      default: throw std::invalid_argument("Invalid render mode");
      }
    }

    inline size_t getNumberSystemElementCount() const {
      switch (numberSystem) {
      case options::NumberSystem::c1: return 2;
      case options::NumberSystem::q: [[fallthrough]];
      case options::NumberSystem::c2: return 4;
      case options::NumberSystem::o: [[fallthrough]];
      case options::NumberSystem::c3: return 8;
      default: throw std::invalid_argument("Invalid number system");
      }
    }

    inline std::string getNumberSystemName() const {
      switch (numberSystem) {
      case options::NumberSystem::c1: return "complex";
      case options::NumberSystem::q: return "quaternion";
      case options::NumberSystem::c2: return "bicomplex";
      case options::NumberSystem::o: return "octonian";
      case options::NumberSystem::c3: return "tricomplex";
      default: throw std::invalid_argument("Invalid number system");
      }
    }

    template<options::Space space>
    inline proxy::Proxy<gpu::opencl::cltypes::Viewspace> auto& getViewspace() {
      if constexpr (space == options::Space::phase) {
        return phaseView;
      } else if constexpr (space == options::Space::dynamical) {
        return dynamicalView;
      } else {
        static_assert(false, "Invalid space");
        return dynamicalView; // Just return something here to avoid extra compiler messages.
      }
    }
  };
}
#endif