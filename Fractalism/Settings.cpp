#include <Fractalism/Settings.hpp>

#include <Fractalism/App.hpp>

namespace fractalism {
  namespace {
    namespace cltypes = gpu::opencl::cltypes;
    using CLSolver = gpu::opencl::CLSolver;
  }
  Settings::Settings() :
    parameter(0.357712765957447, 0.111702127659575, 0.0),
    resolution(64),
    numberSystem(options::NumberSystem::q),
    renderMode(options::RenderMode::translated),
    renderDimensions(options::Dimensions::three),
    space(options::Space::both),
    iterationsPerFrame(100),
    iterationModifier(125.0),
    phaseView(
      -0.5, 0.0, 0.0,
      zoom1x,
      1, 2, 3),
    dynamicalView(
      0.0, 0.0, 0.0,
      zoom1x,
      1, 2, 3),
    trackball(2.0, 1.0, 0.1),
    phaseZMapping(3),
    dynamicalZMapping(3) {
    // Make sure we handle the z-mapping regardless of initial render dimensions
    cltypes::ViewMapping& phaseViewMapping = phaseView.unwrap().mapping;
    cltypes::ViewMapping& dynamicalViewMapping = dynamicalView.unwrap().mapping;
    switch (renderDimensions) {
    case options::Dimensions::two:
      phaseZMapping = phaseViewMapping.z;
      phaseViewMapping.z = 0;
      dynamicalZMapping = dynamicalViewMapping.z;
      dynamicalViewMapping.z = 0;
      break;
    case options::Dimensions::three:
      phaseViewMapping.z = phaseZMapping;
      dynamicalViewMapping.z = phaseZMapping;
      break;
    default:
      return;
    }
  }

  void Settings::parameterChanged() {
    App::get<CLSolver>().updateParameter();
  }

  void Settings::resolutionChanged() {
    App::get<CLSolver>().updateResolution();
  }

  void Settings::numberSystemChanged() {
    App::get<CLSolver>().updateKernels();
  }

  void Settings::renderModeChanged() {
    App::get<CLSolver>().updateKernels();
  }

  void Settings::renderDimensionsChanged() {
    Settings settings = App::get<Settings>();
    cltypes::Viewspace phaseView = settings.phaseView;
    cltypes::Viewspace dynamicalView = settings.dynamicalView;
    switch (settings.renderDimensions) {
    case options::Dimensions::two:
      settings.phaseZMapping = phaseView.mapping.z;
      phaseView.mapping.z = 0;
      settings.dynamicalZMapping = dynamicalView.mapping.z;
      dynamicalView.mapping.z = 0;
      break;
    case options::Dimensions::three:
      phaseView.mapping.z = settings.phaseZMapping;
      dynamicalView.mapping.z = settings.phaseZMapping;
      break;
    default:
      return;
    }
    settings.phaseView = phaseView;
    settings.dynamicalView = dynamicalView;
    resolutionChanged(); // Gotta have 3D textures and buffer.
  }

  void Settings::spaceChanged() {
    App::get<CLSolver>().updateKernels();
  }

  void Settings::iterationsPerFrameChanged() {}

  void Settings::iterationModifierChanged() {
    CLSolver& solver = App::get<CLSolver>();
    solver.updatePhaseView();
    solver.updateDynamicalView();
  }

  void Settings::phaseViewChanged() {
    App::get<CLSolver>().updatePhaseView();
  }

  void Settings::dynamicalViewChanged() {
    App::get<CLSolver>().updateDynamicalView();
  }

  void Settings::trackballChanged() {}
}