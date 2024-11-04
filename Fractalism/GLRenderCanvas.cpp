#include <cmath>
#include <algorithm>
#include "WxIncludeHelper.hpp"
#include <wx/time.h>

#include "GLRenderCanvas.hpp"
#include "Options.hpp"
#include "Events.hpp"
#include "Utils.hpp"

const double NaN = nan("");

static constexpr GLRenderer* renderer(options::Dimensions renderDimensions, GLRenderer2D& renderer2D, GLRenderer3D& renderer3D) {
  switch (renderDimensions) {
  case options::Dimensions::two: return &renderer2D;
  case options::Dimensions::three: return &renderer3D;
  default: throw std::invalid_argument("Invalid render dimension");
  }
}

static constexpr const char* spaceName(const options::Space& space) {
  switch (space) {
  case options::Space::phase: return "Phase Space";
  case options::Space::dynamical: return "Dynamical Space";
  case options::Space::both: return "Phase and Dynamical Spaces";
  default: throw std::invalid_argument("Invalid computation space");
  }
}

GLRenderCanvas::GLRenderCanvas(wxWindow* parent, SharedState::RenderState& renderState, wxStatusBar& statusBar) :
      wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, "GLRenderCanvas"),
      renderState(renderState),
      statusBar(statusBar),
      solverProgram(nullptr),
      renderer2D(renderState),
      renderer3D(renderState),
      activeRenderer(renderer(renderState.sharedState.renderDimensions, renderer2D, renderer3D)),
      lastPoint(NaN) {
  statusBar.SetStatusText(spaceName(renderState.space));
  bindMouseHandler(wxEVT_MOTION);
  bindMouseHandler(wxEVT_LEFT_DOWN);
  bindMouseHandler(wxEVT_LEFT_UP);
  bindMouseHandler(wxEVT_RIGHT_DOWN);
  bindMouseHandler(wxEVT_RIGHT_UP);
  Bind(wxEVT_MOUSE_CAPTURE_LOST, [](wxMouseCaptureLostEvent&){});
  Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent&) {
      lastPoint = glm::dvec2(NaN);
    });
  Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent&) {
      lastPoint = glm::dvec2(NaN);
      this->statusBar.SetStatusText("", 1);
    });
  Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent& evt) {
      double delta = (static_cast<double>(evt.GetWheelRotation()) / static_cast<double>(evt.GetWheelDelta() * 2));
      double oldZoom = SharedState::RenderState::zoom1x / this->renderState.view.value.zoom;
      // Do not pass this to std::clamp as a temporary, otherwise we might get a dangling reference.
      double newZoom = oldZoom + ((delta * oldZoom) / SharedState::RenderState::zoom1x);
      this->renderState.view.value.zoom = SharedState::RenderState::zoom1x / std::clamp(
        newZoom,
        SharedState::RenderState::minZoom,
        SharedState::RenderState::maxZoom);
      this->renderState.view.isDirty = true;
      this->statusBar.SetStatusText(wxString::Format("zoom:%.2f", SharedState::RenderState::zoom1x / this->renderState.view.value.zoom), 2);
    });
}

GLRenderCanvas::~GLRenderCanvas() {
  delete solverProgram;
}

void GLRenderCanvas::render() {
  SharedState& sharedState = renderState.sharedState;
  
  if (!solverProgram) {
    sharedState.prepareGLContext(*this);
    statusBar.SetStatusText(wxString::Format("zoom:%.2f", SharedState::RenderState::zoom1x / renderState.view.value.zoom), 2);
    solverProgram = renderState.createProgram();
  }

  if (activeRenderer->dimension != sharedState.renderDimensions) {
    activeRenderer = renderer(sharedState.renderDimensions, renderer2D, renderer3D);
  }

  CLSolver::Output& solverOutput = renderState.runProgram(solverProgram);
  wxSize size = GetSize();
  sharedState.setGLContext(*this);
  activeRenderer->render(size.GetWidth(), size.GetHeight(), solverOutput.texture);
  SwapBuffers();
}

void GLRenderCanvas::bindMouseHandler(const wxEventTypeTag<wxMouseEvent> &eventType) {
  Bind(eventType, [this](wxMouseEvent& evt) {
    if (evt.ButtonDown() && !HasCapture()) {
      CaptureMouse();
    } else if (evt.ButtonUp() && HasCapture()) {
      ReleaseCapture();
    }
    wxPoint point = evt.GetLogicalPosition(wxClientDC(this));
    wxSize size = GetSize();
    double x = (static_cast<double>(point.x) / static_cast<double>(size.GetWidth()) * 2.0) - 1.0;
    double y = 1.0 - (static_cast<double>(point.y) / static_cast<double>(size.GetHeight()) * 2.0);
    handleMouseEvents(x, y, evt);
  });
}

void GLRenderCanvas::handleMouseEvents(double x, double y, wxMouseEvent& evt) {
  if (evt.Dragging() && evt.RightIsDown() && !isnan(lastPoint.x)) {
    activeRenderer->changeView(lastPoint.x - x, lastPoint.y - y);
    StateChangeEvent::fireEvent(renderState.viewChangeEvent, this);
  }
  cltypes::Viewspace view = renderState.view;
  glm::dvec2 coords = {
    (copysign(1.0, view.mapping.x) * x * view.zoom) + view.center.raw[abs(view.mapping.x) - 1],
    (copysign(1.0, view.mapping.y) * y * view.zoom) + view.center.raw[abs(view.mapping.y) - 1]
  };
  // update parameter
  if (evt.LeftIsDown()) {
    activeRenderer->changeParameter(coords.x, coords.y);
    StateChangeEvent::fireEvent(EVT_ParameterChanged, this);
  }
  statusBar.SetStatusText(wxString::Format("%.15f, %+.15f", coords.x, coords.y), 1);
    
  lastPoint = glm::dvec2(x, y);
}