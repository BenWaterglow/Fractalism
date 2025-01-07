#ifndef _FRACTALISM_SETTINGS_HPP_
#define _FRACTALISM_SETTINGS_HPP_

#include <limits>
#include <vector>

#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism {
  struct Settings {
  public:
    static constexpr double zoom1x = 1.5;
    static constexpr double minZoom = 0.1;
    static constexpr double maxZoom = 1.0e16;
    static constexpr double minIterations = 1.0;
    static constexpr double maxIterations = std::numeric_limits<cl_uint>::max();

    options::NumberSystem numberSystem;
    options::Dimensions renderDimensions;
    cl::NDRange resolution;
    gpu::types::Number parameter;
    std::vector<ViewWindowSettings> viewWindowSettings;

    Settings();

    inline size_t getViewWindowCount() const {
      return viewWindowSettings.size();
    }

    inline size_t getNumberSystemElementCount() const {
      return options::elementCount(numberSystem);
    }

    inline options::Dimensions& setRenderDimensions(options::Dimensions renderDimensions) {
      this->renderDimensions = renderDimensions;
      setResolution(resolution[0]);
      return this->renderDimensions;
    }

    inline cl::NDRange& setResolution(cl::size_type resolution) {
      switch(renderDimensions) {
      case options::Dimensions::two: return this->resolution = cl::NDRange(resolution, resolution);
      case options::Dimensions::three: return this->resolution = cl::NDRange(resolution, resolution, resolution);
      default: throw AssertionError("Invalid render dimension");
      }
    }
  };
}
#endif