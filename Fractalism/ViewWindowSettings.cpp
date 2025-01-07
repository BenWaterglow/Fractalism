#include <Fractalism/ViewWindowSettings.hpp>
#include <Fractalism/Settings.hpp>

namespace fractalism {
  ViewWindowSettings::ViewWindowSettings(options::Space space, options::RenderMode renderMode) : 
        space(space),
        renderMode(renderMode),
        iterationModifier(125.0),
        iterationsPerFrame(100),
        view(space == options::Space::phase ? -0.5 : 0.0, 0.0, 0.0),
        camera(2.0, 1.0, 0.1) {}
}