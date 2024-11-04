#ifndef _SHARED_STATE_HPP_
#define _SHARED_STATE_HPP_

#include "WxIncludeHelper.hpp"

// we don't use glew in this header, but it must be included before gl.h
#include <GL/glew.h>
#include <wx/glcanvas.h>

#include "CLSolver.hpp"
#include "CLTypes.hpp"
#include "GLShaderProgram.hpp"
#include "GPUContext.hpp"
#include "Options.hpp"
#include "Settings.hpp"
#include "Events.hpp"
#include "Utils.hpp"

class SharedState {
private:
  struct Handler {
    void parameterChanged(cltypes::Number& parameter);
    void resolutionChanged(int& resolution);
    void numberSystemChanged(options::NumberSystem& numberSystem);
    void renderModeChanged(options::RenderMode& renderMode);
    void renderDimensionsChanged(options::Dimensions& renderDimensions);
    void iterationsPerFrameChanged(int& iterationsPerFrame);
    void iterationModifierChanged(double& iterationModifier);
    void phaseViewChanged(cltypes::Viewspace& phaseView);
    void dynamicalViewChanged(cltypes::Viewspace& dynamicalView);
  };
public:
  class RenderState {
  public:
    static const double zoom1x;
    static const double minZoom;
    static const double maxZoom;
    static const double minIterations;
    static const double maxIterations;
    RenderState(SharedState& sharedState, options::Space space, const wxEventTypeTag<StateChangeEvent>& viewChangeEvent);
    ~RenderState();
    SharedState& sharedState;
    CLSolver::Program* createProgram();
    CLSolver::Output& runProgram(CLSolver::Program* program);
    utils::Dirtyable<cltypes::Viewspace> view;
    cl_char lastZMapping;
    utils::Dirtyable<int&> resolution;
    utils::Dirtyable<options::NumberSystem&> numberSystem;
    const options::Space space;
    const wxEventTypeTag<StateChangeEvent>& viewChangeEvent;
  };
  SharedState();
  ~SharedState();
  void prepareGLContext(wxGLCanvas& canvas);
  void setGLContext(wxGLCanvas& canvas) const;
  GLShaderProgram& getShader(const options::Dimensions& renderDimensions);
  void reloadShaders();
  wxStatusBar* mainStatusBar;
  double iterationModifier;
  int iterationsPerFrame;
  options::Dimensions renderDimensions;
  options::NumberSystem numberSystem;
  utils::Dirtyable<cltypes::Number> parameter;
  utils::Dirtyable<int> resolution;
  RenderState phase;
  RenderState dynamical;
  Settings<Handler> settings;
private:
  struct GPU {
    GPU(wxGLCanvas& canvas, wxStatusBar& statusBar);
    GPUContext ctx;
    CLSolver solver;
  };
  GPU* gpu;
  Handler handler;
  GLShaderProgram shader2D;
  GLShaderProgram shader3D;
};

#endif