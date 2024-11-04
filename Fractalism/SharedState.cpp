#include <algorithm>

#include "SharedState.hpp"
#include "Exceptions.hpp"
#include "Utils.hpp"
#include "Settings.hpp"
#include "GLRenderer.hpp"

const double SharedState::RenderState::zoom1x = 1.5;
const double SharedState::RenderState::minZoom = 0.1;
const double SharedState::RenderState::maxZoom = 10000000000000000.0;
const double SharedState::RenderState::minIterations = 1;
const double SharedState::RenderState::maxIterations = INT_MAX;


SharedState::SharedState() :
  mainStatusBar(nullptr),
  gpu(nullptr),
  iterationModifier(125.0),
  iterationsPerFrame(100),
  renderDimensions(options::Dimensions::three),
  numberSystem(options::NumberSystem::c2),
  parameter(),
  resolution(64),
  phase(*this, options::Space::phase, EVT_PhaseViewChanged),
  dynamical(*this, options::Space::dynamical, EVT_DynamicalViewChanged),
  settings(handler) {

  // settings.parameter = parameter.value;
  // settings.resolution = resolution.value;
}

SharedState::~SharedState() {
  delete gpu;
}

SharedState::RenderState::RenderState(SharedState& sharedState, options::Space space, const wxEventTypeTag<StateChangeEvent>& viewChangeEvent) :
  space(space),
  sharedState(sharedState),
  viewChangeEvent(viewChangeEvent),
  resolution(sharedState.resolution.value),
  numberSystem(sharedState.numberSystem),
  // offset the initial view for phase space
  view(
    space == options::Space::phase ? -0.5 : 0.0,
    0.0,
    0.0,
    zoom1x / 1.0,
    1,
    2,
    sharedState.renderDimensions == options::Dimensions::two ? 0 : 3),
  lastZMapping(3) {}

SharedState::RenderState::~RenderState() {}

CLSolver::Program* SharedState::RenderState::createProgram() {
  return sharedState.gpu->solver.createProgram(space);
}

CLSolver::Output& SharedState::RenderState::runProgram(CLSolver::Program* program) {
  if (resolution.isDirty || !program->isPrepared()) {
    program->prepare(sharedState.renderDimensions, resolution);
    resolution.isDirty = false;
    numberSystem.isDirty = true;
    view.isDirty = true;
  }

  if (numberSystem.isDirty) {
    program->setNumberSystem(numberSystem);
    numberSystem.isDirty = false;
    view.isDirty = true;
  }

  if (view.isDirty || space == options::Space::dynamical && sharedState.parameter.isDirty) {
    // Do not pass this to std::clamp as a temporary, otherwise we might get a dangling reference.
    double iterations = sharedState.iterationModifier * pow(2.0, log10((SharedState::RenderState::zoom1x / view.value.zoom) / 2.0));
    program->setParameters(
      static_cast<int>(std::clamp(
        iterations,
        SharedState::RenderState::minIterations,
        SharedState::RenderState::maxIterations)),
      sharedState.iterationsPerFrame,
      view,
      sharedState.parameter);
    view.isDirty = false;
    if (space == options::Space::dynamical) {
      sharedState.parameter.isDirty = false;
    }
  }

  return program->execute();
}

void SharedState::prepareGLContext(wxGLCanvas& canvas) {
  if (!gpu) {
    gpu = new GPU(canvas, *mainStatusBar);
    shader2D = GLShaderProgram("Shaders/2d.vert", "Shaders/2d.frag");
    shader3D = GLShaderProgram("Shaders/3d.vert", "Shaders/3d.frag");
    reloadShaders();
  }
}

void SharedState::setGLContext(wxGLCanvas& canvas) const {
  canvas.SetCurrent(gpu->ctx.glCtx);
}

GLShaderProgram& SharedState::getShader(const options::Dimensions& dimensions) {
  switch (dimensions) {
  case options::Dimensions::two:   return shader2D;
  case options::Dimensions::three: return shader3D;
  default: throw std::invalid_argument("Invalid render dimension.");
  }
}

void SharedState::reloadShaders() {
  mainStatusBar->PushStatusText("Loading shaders...");
  shader2D.load();
  shader3D.load();
  mainStatusBar->PopStatusText();
}

SharedState::GPU::GPU(wxGLCanvas& canvas, wxStatusBar& statusBar) : ctx(canvas, statusBar), solver(ctx, statusBar) {}

inline void SharedState::Handler::parameterChanged(cltypes::Number& parameter) {}

inline void SharedState::Handler::resolutionChanged(int& resolution) {}

inline void SharedState::Handler::numberSystemChanged(options::NumberSystem& numberSystem) {}

inline void SharedState::Handler::renderModeChanged(options::RenderMode& renderMode) {}

inline void SharedState::Handler::renderDimensionsChanged(options::Dimensions& renderDimensions) {}

inline void SharedState::Handler::iterationsPerFrameChanged(int& iterationsPerFrame) {}

inline void SharedState::Handler::iterationModifierChanged(double& iterationModifier) {}

inline void SharedState::Handler::phaseViewChanged(cltypes::Viewspace& phaseView) {}

inline void SharedState::Handler::dynamicalViewChanged(cltypes::Viewspace& dynamicalView) {}
