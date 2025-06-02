#include <format>
#include <cmath>
#include <numbers>
#include <algorithm>
#include <glm/glm.hpp>

#include <Fractalism/UI/GLRenderCanvas.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/Events.hpp>
#include <Fractalism/App.hpp>

namespace fractalism::ui {
  namespace {
    namespace types = gpu::types;
    static inline types::Coordinates pointCoordinates(wxPoint point, wxSize size) {
      return {
        (static_cast<real>(point.x) / static_cast<real>(size.GetWidth()) * 2.0) - 1.0,
        1.0 - (static_cast<real>(point.y) / static_cast<real>(size.GetHeight()) * 2.0) // point.y is inverted, ie, positive is down.
      };
    }
  }

  GLRenderCanvas::GLRenderCanvas(wxWindow& parent, ViewWindowSettings& settings, wxStatusBar& statusBar) :
        wxGLCanvas(
          &parent,
          wxID_ANY,
          nullptr,
          wxDefaultPosition,
          wxDefaultSize,
          wxFULL_REPAINT_ON_RESIZE,
          "GLRenderCanvas"),
        lastPoint(types::Coordinates::none) {

    SetCursor(*wxCROSS_CURSOR);
    const auto clearLastPoint = [this](wxMouseEvent&) { setLastPoint(types::Coordinates::none); };
    Bind(wxEVT_ENTER_WINDOW, clearLastPoint);
    Bind(wxEVT_LEAVE_WINDOW, clearLastPoint);

    Bind(wxEVT_MOUSE_CAPTURE_LOST, [](wxMouseCaptureLostEvent&) {}); // No-op. Just need to suppress the event.

    Bind(wxEVT_MOUSEWHEEL, [this, &settings](wxMouseEvent& evt) {
      real delta = -static_cast<real>(evt.GetWheelRotation()) / static_cast<real>(evt.GetWheelDelta() * 2);
      switch(App::get<Settings>().renderDimensions) {
      case options::Dimensions::two: {
        settings.view.zoom = std::clamp(
          std::exp(std::log(settings.view.zoom) + delta),
          gpu::types::Viewspace::minZoom,
          gpu::types::Viewspace::maxZoom);
        events::ZoomChanged::fire(this, settings.view);
        break;
      }
      case options::Dimensions::three: {
        settings.camera += delta;
        break;
      }
      default: throw std::invalid_argument("Invalid render dimension");
      }
    });

    const auto mouseHandler = [this, &settings](wxMouseEvent& evt) {
      if (evt.ButtonDown() && !HasCapture()) {
        CaptureMouse();
      } else if (evt.ButtonUp() && HasCapture()) {
        ReleaseCapture();
      }
      types::Coordinates currentPoint = pointCoordinates(evt.GetPosition(), GetSize());
      if (evt.Dragging() && evt.RightIsDown() && lastPoint) {
        types::Coordinates delta = lastPoint - currentPoint;
        switch (App::get<Settings>().renderDimensions) {
        case options::Dimensions::two: {
          settings.view += delta;
          events::ViewCenterChanged::fire(this, settings.view);
          break;
        }
        case options::Dimensions::three: {
          settings.camera += delta;
          break;
        }
        default: throw std::invalid_argument("Invalid render dimension");
        }
      }
      // update parameter
      if (evt.LeftIsDown()) {
        switch (App::get<Settings>().renderDimensions) {
        case options::Dimensions::two: {
          gpu::types::Number& parameter = App::get<Settings>().parameter;
          parameter = settings.view + currentPoint;
          events::ParameterChanged::fire(this, parameter);
          break;
        }
        case options::Dimensions::three: {
          // TODO: How can we get a parameter from a 3D view? Ray-march with a threshold?
          break;
        }
        default: throw std::invalid_argument("Invalid render dimensions");
        }
      }
      setLastPoint(currentPoint);
    };
    Bind(wxEVT_LEFT_DOWN, mouseHandler);
    Bind(wxEVT_RIGHT_DOWN, mouseHandler);
    Bind(wxEVT_LEFT_UP, mouseHandler);
    Bind(wxEVT_RIGHT_UP, mouseHandler);
    Bind(wxEVT_MOTION, mouseHandler);
  }

  void GLRenderCanvas::setLastPoint(const types::Coordinates& coordinates) {
    lastPoint = coordinates;
    events::CoordinatesChanged::fire(this, coordinates);
  }
}