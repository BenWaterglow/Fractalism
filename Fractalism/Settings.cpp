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
    trackball(2.0, 1.0, 0.1) {}

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
    resolutionChanged(); // Gotta have 3D textures and buffer.
    phaseViewChanged();
    dynamicalViewChanged();
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