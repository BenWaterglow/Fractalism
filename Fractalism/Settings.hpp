#ifndef _FRACTALISM_SETTINGS_HPP_
#define _FRACTALISM_SETTINGS_HPP_

#include <limits>
#include <vector>

#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism {

/**
 * @struct Settings
 * @brief Manages the settings for the Fractalism application, including zoom,
 * iterations, and rendering modes.
 */
struct Settings {
public:
  static constexpr double minIterations = 1.0;  ///< Minimum number of iterations.
  static constexpr double maxIterations = std::numeric_limits<cl_uint>::max(); ///< Maximum number of iterations.

  options::NumberSystem numberSystem;                 ///< The number system setting.
  options::Dimensions renderDimensions;               ///< The render dimensions setting.
  cl::NDRange resolution;                             ///< The resolution setting.
  gpu::types::Number parameter;                       ///< The fractal parameter.
  std::vector<ViewWindowSettings> viewWindowSettings; ///< The view window settings.

  /**
   * @brief Constructs a Settings object with default values.
   */
  Settings();

  /**
   * @brief Gets the number of view windows.
   * @return The number of view windows.
   */
  inline size_t getViewWindowCount() const { return viewWindowSettings.size(); }

  /**
   * @brief Gets the number of elements in the number system.
   * @return The number of elements in the number system.
   */
  inline size_t getNumberSystemElementCount() const {
    return options::elementCount(numberSystem);
  }

  /**
   * @brief Sets the render dimensions and updates the resolution accordingly.
   * @param renderDimensions The new render dimensions.
   * @return A reference to the updated render dimensions.
   */
  inline options::Dimensions& setRenderDimensions(options::Dimensions renderDimensions) {
    this->renderDimensions = renderDimensions;
    setResolution(resolution[0]);
    return this->renderDimensions;
  }

  /**
   * @brief Sets the resolution based on the render dimensions.
   * @param resolution The new resolution.
   * @return A reference to the updated resolution.
   */
  inline cl::NDRange& setResolution(cl::size_type resolution) {
    switch (renderDimensions) {
    case options::Dimensions::two:
      return this->resolution = cl::NDRange(resolution, resolution);
    case options::Dimensions::three:
      return this->resolution = cl::NDRange(resolution, resolution, resolution);
    default:
      throw AssertionError("Invalid render dimension");
    }
  }
};
} // namespace fractalism

#endif
