#include <cmath>
#include <format>
#include <Fractalism/UI/UI.hpp>
#include <Fractalism/App.hpp>
#include <Fractalism/Exceptions.hpp>
#include <Fractalism/UI/MenuBar.hpp>
#include <Fractalism/UI/Controls/ParameterToolBar.hpp>
#include <Fractalism/UI/Controls/NumberSystemToolBar.hpp>
#include <Fractalism/UI/Controls/RenderSettingsToolBar.hpp>

template <>
struct std::formatter<cl::NDRange> {

  constexpr auto parse(std::format_parse_context& ctx) const {
    return ctx.end();
  }

  auto format(cl::NDRange range, std::format_context& ctx) const {
    switch(range.dimensions()) {
    case 0: return std::format_to(ctx.out(), "0");
    case 1: return std::format_to(ctx.out(), "{}", range[0]);
    case 2: return std::format_to(ctx.out(), "{}x{}", range[0], range[1]);
    case 3: return std::format_to(ctx.out(), "{}x{}x{}", range[0], range[1], range[2]);
    default: throw fractalism::AssertionError(std::format("Invalid range dimensions: {}", range.dimensions()));
    }
  }
};

namespace fractalism::ui {

  UI::UI() :
          wxFrame(nullptr, wxID_ANY, "Fractalism"),
          lastRenderMillis(wxGetUTCTimeMillis()),
          fps(0.0),
          frameManager(this, wxAUI_MGR_DEFAULT | wxAUI_MGR_LIVE_RESIZE) {
    SetMenuBar(new MenuBar());
    CreateStatusBar(5);

    frameManager.SetManagedWindow(this);

    controls::ParameterToolBar& parameterToolBar = *new controls::ParameterToolBar(*this);
    frameManager.AddPane(&parameterToolBar, wxAuiPaneInfo()
      .ToolbarPane()
      .Name("Parameter")
      .Caption("Parameter")
      .CaptionVisible(true)
      .CloseButton(false)
      .Gripper(false)
      .Dock()
      .Right()
      .Layer(1).Row(1).Position(1));
    controls::NumberSystemToolBar& numberSystemToolBar = *new controls::NumberSystemToolBar(*this);
    frameManager.AddPane(&numberSystemToolBar, wxAuiPaneInfo()
      .ToolbarPane()
      .Name("Number System")
      .Caption("Number System")
      .CaptionVisible(true)
      .CloseButton(false)
      .Gripper(false)
      .Dock()
      .Right()
      .Layer(1).Row(1).Position(2));
    controls::RenderSettingsToolBar &renderSettingsToolBar = *new controls::RenderSettingsToolBar(*this);
    frameManager.AddPane(&renderSettingsToolBar, wxAuiPaneInfo()
      .ToolbarPane()
      .Name("Render Settings")
      .Caption("Render Settings")
      .CaptionVisible(true)
      .CloseButton(false)
      .Gripper(false)
      .Dock()
      .Right()
      .Layer(1).Row(1).Position(3));

    // Create the rendering windows, using the ViewWindowSettings instances
    // to track how many windows to create, and how to configure them.
    size_t viewWindowCount = App::get<Settings>().getViewWindowCount();
    size_t gridSize = static_cast<size_t>(std::ceil(std::sqrt(static_cast<double>(viewWindowCount))));
    std::vector<ViewWindow*>& viewWindows = this->viewWindows;
    viewWindows.reserve(viewWindowCount);
    int width = -1;
    int height = -1;
    for (size_t i = 0; i < viewWindowCount; i++) {
      ViewWindow* viewWindow = new ViewWindow(*this, i);
      // Only get the width and height of the first window, and use that for all.
      if (width == -1) {
        viewWindow->GetBestSize(&width, &height);
        height += 50; // Add a little extra height for the toolbar.
        width += height; // Make the window square and allow the gl canvas to expand.
      }
      frameManager.AddPane(viewWindow, wxAuiPaneInfo()
        .Name(viewWindow->GetName())
        .Caption(viewWindow->GetName())
        .CaptionVisible(true)
        .DestroyOnClose(false)
        .Dockable(false)
        .FloatingSize(width, height)
        .FloatingPosition(width * (i % gridSize), height * (i / gridSize)) // Stagger the floating windows
        .Float());
      viewWindows.push_back(viewWindow);
      viewWindow->Bind(events::ParameterChanged::tag, [&viewWindows, &parameterToolBar](events::ParameterChanged::eventType& event) {
        for (ViewWindow* viewWindow : viewWindows) {
          viewWindow->updateParameter();
        }
        parameterToolBar.updateParameter();
      });
    }
    
    parameterToolBar.Bind(events::ParameterChanged::tag, [&viewWindows](events::ParameterChanged::eventType& event) {
      for (ViewWindow* viewWindow : viewWindows) {
          viewWindow->updateParameter();
        }
    });
    numberSystemToolBar.Bind(events::NumberSystemChanged::tag, [&viewWindows](events::NumberSystemChanged::eventType& event) {
      for (ViewWindow* viewWindow : viewWindows) {
        viewWindow->updateNumberSystem();
      }
    });
    renderSettingsToolBar.Bind(events::RenderDimensionsChanged::tag, [this](events::RenderDimensionsChanged::eventType& event) {
      App::get<gpu::opencl::ProgramManager>().updateResolution();
      for (ViewWindow* viewWindow : this->viewWindows) {
        viewWindow->updateRenderDimensions();
      }
      SetStatusText(std::format("Resolution: {}", App::get<Settings>().resolution), 3);
    });
    renderSettingsToolBar.Bind(events::ResolutionChanged::tag, [this](events::ResolutionChanged::eventType& event) {
      App::get<gpu::opencl::ProgramManager>().updateResolution();
      for (ViewWindow* viewWindow : this->viewWindows) {
        viewWindow->updateRenderDimensions();
      }
      SetStatusText(std::format("Resolution: {}", event.getValue()), 3);
    });
    Bind(wxEVT_IDLE, [this](wxIdleEvent &evt) {
      App::render(this->viewWindows);
      updateFps();
      evt.RequestMore();
    });
    SetStatusText("Fractalism");
    SetStatusText(std::format("Resolution: {}", App::get<Settings>().resolution), 3);
    frameManager.Update();
    wxSize size = GetBestSize();
    SetSize(size);
    wxSize displaySize = wxGetDisplaySize();
    SetPosition(wxPoint(displaySize.x - size.x, 0));
  }

  void UI::updateFps() {
    wxLongLong now = wxGetUTCTimeMillis();
    wxLongLong delta = now - lastRenderMillis;
    lastRenderMillis = now;
    const double fpsSmoothing = 0.7;
    fps = (fps * fpsSmoothing) + ((1.0 / (delta.ToDouble() / 1000.0)) * (1.0 - fpsSmoothing));
    if (isnan(fps) || isinf(fps)) {
      fps = 0.0;
    }
    SetStatusText(std::format("FPS: {:.2f}", fps), 4);
  }
}