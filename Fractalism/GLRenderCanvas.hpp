#ifndef _GL_RENDER_CANVAS_HPP_
#define _GL_RENDER_CANVAS_HPP_

// we don't use glew in this header, but it must be included before gl.h, which is included by wx/glcanvas.h
#include <GL/glew.h>
#include "WxIncludeHelper.hpp"
#include <wx/glcanvas.h>

#include <glm/glm.hpp>

#include "SharedState.hpp"
#include "GLRenderer.hpp"

class GLRenderCanvas : public wxGLCanvas {
public:
  GLRenderCanvas(wxWindow* parent, SharedState::RenderState& renderState, wxStatusBar& statusBar);
  ~GLRenderCanvas();
  void render();
private:
  void bindMouseHandler(const wxEventTypeTag<wxMouseEvent>& eventType);
  void handleMouseEvents(double x, double y, wxMouseEvent& evt);
  SharedState::RenderState& renderState;
  wxStatusBar& statusBar;
  CLSolver::Program* solverProgram;
  GLRenderer2D renderer2D;
  GLRenderer3D renderer3D;
  GLRenderer* activeRenderer;
  glm::dvec2 lastPoint;
};

#endif