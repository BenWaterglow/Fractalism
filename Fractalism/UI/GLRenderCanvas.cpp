#include <format>
#include <cmath>
#include <algorithm>
#include <wx/time.h>

#include <Fractalism/UI/GLRenderCanvas.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/Events.hpp>
#include <Fractalism/Proxy.hpp>
#include <Fractalism/App.hpp>

namespace fractalism {
  namespace ui {
    namespace {
      namespace cltypes = gpu::opencl::cltypes;
    }
    const double NaN = nan("");

    GLRenderCanvas::GLRenderCanvas(wxWindow & parent, options::Space space, wxStatusBar & statusBar) :
      wxGLCanvas(&parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, "GLRenderCanvas"),
      statusBar(statusBar),
      lastPoint(NaN),
      viewChangeEvent(space == options::Space::phase ? EVT_PhaseViewChanged : EVT_DynamicalViewChanged) {
      proxy::Proxy<cltypes::Viewspace> auto& viewspace = App::get<Settings>().getViewspace<options::Space::phase>();
      bindMouseHandler(wxEVT_MOTION, viewspace);
      bindMouseHandler(wxEVT_LEFT_DOWN, viewspace);
      bindMouseHandler(wxEVT_LEFT_UP, viewspace);
      bindMouseHandler(wxEVT_RIGHT_DOWN, viewspace);
      bindMouseHandler(wxEVT_RIGHT_UP, viewspace);
      Bind(wxEVT_MOUSE_CAPTURE_LOST, [](wxMouseCaptureLostEvent&) {});
      Bind(wxEVT_ENTER_WINDOW, [&lastPoint = this->lastPoint](wxMouseEvent&) {
        lastPoint = glm::dvec2(NaN);
        });
      Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent&) {
        lastPoint = glm::dvec2(NaN);
        this->statusBar.SetStatusText("", 1);
        });
      Bind(wxEVT_MOUSEWHEEL, [&viewspace, &statusBar](wxMouseEvent& evt) {
        cltypes::Viewspace view = viewspace;
        double delta = (static_cast<double>(evt.GetWheelRotation()) / static_cast<double>(evt.GetWheelDelta() * 2));
        double oldZoom = Settings::zoom1x / view.zoom;
        view.zoom = Settings::zoom1x / std::clamp(
          oldZoom + ((delta * oldZoom) / Settings::zoom1x),
          Settings::minZoom,
          Settings::maxZoom);
        statusBar.SetStatusText(std::format("zoom: {:.2f}", Settings::zoom1x / view.zoom), 2);
        viewspace = view;
        });
    }

    void GLRenderCanvas::bindMouseHandler(const wxEventTypeTag<wxMouseEvent>&eventType, proxy::Proxy<cltypes::Viewspace> auto& viewspace) {
      Bind(eventType, [this, &viewspace](wxMouseEvent& evt) {
        if (evt.ButtonDown() && !HasCapture()) {
          CaptureMouse();
        }
        else if (evt.ButtonUp() && HasCapture()) {
          ReleaseCapture();
        }
        wxPoint point = evt.GetLogicalPosition(wxClientDC(this));
        wxSize size = GetSize();
        double x = (static_cast<double>(point.x) / static_cast<double>(size.GetWidth()) * 2.0) - 1.0;
        double y = 1.0 - (static_cast<double>(point.y) / static_cast<double>(size.GetHeight()) * 2.0);
        handleMouseEvents(x, y, evt, viewspace);
        });
    }

    void GLRenderCanvas::handleMouseEvents(double x, double y, wxMouseEvent & evt, proxy::Proxy<cltypes::Viewspace> auto& viewspace) {
      Settings& settings = App::get<Settings>();
      cltypes::Viewspace view = viewspace;
      if (evt.Dragging() && evt.RightIsDown() && !isnan(lastPoint.x)) {
        glm::dvec2 delta = lastPoint - glm::dvec2(x, y);
        switch (settings.renderDimensions) {
        case options::Dimensions::two: {
          view.center.raw[abs(view.mapping.x) - 1] += (copysign(view.zoom, view.mapping.x) * delta.x);
          view.center.raw[abs(view.mapping.y) - 1] += (copysign(view.zoom, view.mapping.y) * delta.y);
          viewspace = view;
          StateChangeEvent::fireEvent(viewChangeEvent, this);
          break;
        }
        case options::Dimensions::three: {
          glm::dvec3 trackball = settings.trackball;
          trackball.t -= delta.x;
          trackball.p += delta.y;
          settings.trackball = trackball;
          break;
        }
        default: throw std::invalid_argument("Invalid render dimension");
        }
      }
      glm::dvec2 coords = {
        view.center.raw[abs(view.mapping.x) - 1] + (copysign(view.zoom, view.mapping.x) * x),
        view.center.raw[abs(view.mapping.y) - 1] + (copysign(view.zoom, view.mapping.y) * y)
      };
      // update parameter
      if (evt.LeftIsDown()) {
        switch (settings.renderDimensions) {
        case options::Dimensions::two: {
          cltypes::Number center = view.center;
          center.raw[abs(view.mapping.x) - 1] = coords.x;
          center.raw[abs(view.mapping.y) - 1] = coords.y;
          settings.parameter = center;
          StateChangeEvent::fireEvent(EVT_ParameterChanged, this);
          break;
        }
        case options::Dimensions::three: break;
        default: throw std::invalid_argument("Invalid render dimensions");
        }
      }
      statusBar.SetStatusText(std::format("{:.15f}, {:+.15f}", coords.x, coords.y), 1);

      lastPoint = glm::dvec2(x, y);
    }
  }
}