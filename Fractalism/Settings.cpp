#include <Fractalism/Settings.hpp>

#include <Fractalism/App.hpp>

namespace fractalism {
  Settings::Settings() :
        parameter(0.357712765957447, 0.111702127659575, 0.0),
        numberSystem(options::NumberSystem::quaternion),
        renderDimensions(options::Dimensions::two),
        resolution(64, 64),
        viewWindowSettings{
                ViewWindowSettings(options::Space::phase, options::RenderMode::escape),
                ViewWindowSettings(options::Space::phase, options::RenderMode::translated),
                ViewWindowSettings(options::Space::dynamical, options::RenderMode::escape),
                ViewWindowSettings(options::Space::dynamical, options::RenderMode::translated)
        } {}
}