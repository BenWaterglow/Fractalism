#include <Fractalism/UI/ViewWindow.hpp>

#include<format>

#include <Fractalism/App.hpp>
#include <Fractalism/Events.hpp>
#include <Fractalism/Settings.hpp>
#include <Fractalism/Options.hpp>

namespace fractalism::ui {
  ViewWindow::ViewWindow(wxWindow& parent, size_t index) :
      wxPanel(&parent),
      kernel(index),
      statusBar(*new wxStatusBar(this, wxID_ANY, wxSTB_SHOW_TIPS | wxSTB_ELLIPSIZE_END | wxFULL_REPAINT_ON_RESIZE)),
      renderCanvas(*new GLRenderCanvas(*new wxPanel(this), kernel.settings, statusBar)),
      viewspaceToolBar(*new controls::ViewspaceToolBar(*this, kernel.settings.view)),
      iterationToolBar(*new controls::IterationToolBar(*this, kernel.settings)),
      auiManager(this, wxAUI_MGR_DEFAULT | wxAUI_MGR_LIVE_RESIZE) {
    auiManager.SetManagedWindow(this);
    wxWindow* renderCanvasParent = renderCanvas.GetParent();
    wxSizer *renderCanvasSizer = new wxBoxSizer(wxHORIZONTAL);
    renderCanvasSizer->Add(&renderCanvas, 1, wxSHAPED | wxALIGN_CENTER);
    renderCanvasParent->SetSizerAndFit(renderCanvasSizer);
    auiManager.AddPane(renderCanvasParent, wxAuiPaneInfo()
      .CenterPane()
      .Name(options::name(kernel.settings.space) + " " + options::name(kernel.settings.renderMode))
      .Caption(options::name(kernel.settings.space) + " " + options::name(kernel.settings.renderMode))
      .CaptionVisible(true)
      .CloseButton(false)
      .Gripper(false)
      .Movable(false)
      .MaximizeButton(false)
      .Floatable(true));
    auiManager.AddPane(&viewspaceToolBar, wxAuiPaneInfo()
      .ToolbarPane()
      .Name("Viewspace")
      .Caption("Viewspace")
      .CaptionVisible(true)
      .Dock()
      .Left()
      .CloseButton(false)
      .Gripper(false)
      .Floatable(false)
      .Layer(1).Row(1).Position(1));
    auiManager.AddPane(&iterationToolBar, wxAuiPaneInfo()
      .ToolbarPane()
      .Name("Iterations")
      .Caption("Iterations")
      .CaptionVisible(true)
      .Dock()
      .Left()
      .CloseButton(false)
      .Gripper(false)
      .Floatable(false)
      .Layer(1).Row(1).Position(2));
    statusBar.SetFieldsCount(4);
    auiManager.AddPane(&statusBar, wxAuiPaneInfo()
      .ToolbarPane()
      .CaptionVisible(false)
      .DockFixed()
      .Bottom()
      .MinSize(statusBar.GetBestSize())
      .CloseButton(false)
      .PaneBorder(false)
      .Gripper(false)
      .Floatable(false));
    renderCanvas.Bind(events::CoordinatesChanged::tag, [&statusBar = this->statusBar](events::CoordinatesChanged::eventType& event) {
      const gpu::types::Coordinates& coords = event.getValue();
      if (coords) {
        statusBar.SetStatusText(std::format("{:.15f} {:.15f}", coords.x, coords.y), 0);
      } else {
        statusBar.SetStatusText("", 0);
      }
    });
    renderCanvas.Bind(events::ViewCenterChanged::tag, [this](events::ViewCenterChanged::eventType& event) {
      kernel.updateView();
      viewspaceToolBar.updateCenter();
      event.Skip(); // Pass the event up after updating the tool bar.
    });
    renderCanvas.Bind(events::ZoomChanged::tag, [this](events::ZoomChanged::eventType& event) {
      kernel.updateView();
      viewspaceToolBar.updateZoom();
      statusBar.SetStatusText(std::format("zoom: {:.2f}", Settings::zoom1x / event.getValue()), 1);
      event.Skip(); // Pass the event up after updating the tool bar and displaying.
    });
    viewspaceToolBar.Bind(events::ZoomChanged::tag, [this](events::ZoomChanged::eventType& event) {
      kernel.updateView();
      statusBar.SetStatusText(std::format("zoom: {:.2f}", Settings::zoom1x / event.getValue()), 1);
      event.Skip(); // Pass the event up after displaying the zoom
    });
    iterationToolBar.Bind(events::IterationModifierChanged::tag, [this](events::IterationModifierChanged::eventType& event) {
      statusBar.SetStatusText(std::format("Iteration Modifier: {:.4f}", event.getValue()), 2);
      event.Skip(); // Pass it on up after display
    });
    iterationToolBar.Bind(events::IterationsPerFrameChanged::tag, [this](events::IterationsPerFrameChanged::eventType& event) {
      statusBar.SetStatusText(std::format("Iterations Per Frame: {}", event.getValue()), 3);
      event.Skip(); // Pass it on up after display
    });
    // TODO: implement center parameter, screenshot, and video capture tools.

    // NOTE: do NOT call this->update*() functions here. They need GPU context.
    viewspaceToolBar.updateCenter();
    viewspaceToolBar.updateViewMapping();
    viewspaceToolBar.updateZoom();
    viewspaceToolBar.updateNumberSystem();
    viewspaceToolBar.updateRenderDimensions();
    // These are fine. They are picked up on kernel execution.
    updateIterationModifier();
    updateIterationsPerFrame();
    auiManager.Update();
  }

  void ViewWindow::init() {
    kernel.updateKernel();
  }

  void ViewWindow::maybeClearTexture() {
    if (kernel.settings.renderMode == options::RenderMode::translated
        && IsShownOnScreen()
        && kernel.needsMore()) {
      kernel.texture.clear();
    }
  }

  void ViewWindow::enqueueRender(std::vector<cl::Event>& waitEvents) {
    if (IsShownOnScreen()) {
      glFinish();
      if (kernel.needsMore()) {
        kernel.enqueue(waitEvents).wait();
      }
      App::get<gpu::GPU>().renderer.render(kernel.settings, renderCanvas, kernel.texture);
    }
  }

  void ViewWindow::updateParameter() {
    kernel.updateParameter();
  }

  void ViewWindow::updateCenter() {
    kernel.updateView();
    viewspaceToolBar.updateCenter();
  }

  void ViewWindow::updateViewMapping() {
    kernel.updateView();
    viewspaceToolBar.updateViewMapping();
  }

  void ViewWindow::updateZoom() {
    kernel.updateView();
    viewspaceToolBar.updateZoom();
    statusBar.SetStatusText(std::format("zoom: {:.2f}", Settings::zoom1x / kernel.settings.view.zoom), 1);
  }

  void ViewWindow::updateNumberSystem() {
    kernel.updateKernel();
    viewspaceToolBar.updateNumberSystem();
  }

  void ViewWindow::updateRenderDimensions() {
    kernel.updateResolution();
    viewspaceToolBar.updateRenderDimensions();
  }

  void ViewWindow::updateIterationModifier() {
    iterationToolBar.updateIterationModifier();
    statusBar.SetStatusText(std::format("Iteration Modifier: {:.4f}", kernel.settings.iterationModifier), 2);
  }

  void ViewWindow::updateIterationsPerFrame() {
    iterationToolBar.updateIterationsPerFrame();
    statusBar.SetStatusText(std::format("Iterations Per Frame: {}", kernel.settings.iterationsPerFrame), 3);
  }
}