#include <Fractalism/App.hpp>

wxIMPLEMENT_APP(fractalism::App);

namespace fractalism {
  App::App() :
    ui(nullptr),
    gpu(nullptr),
    settings() {}

  App::~App() {
    delete gpu;
  }

  bool App::OnInit() {
    ui = new UI();
    ui->Show(true);
    return true;
  }

  void App::setupGPUContext(wxGLCanvas& canvas) {
    if (!gpu) {
      gpu = new GPU(canvas);
    }
  }

  gpu::opencl::CLSolver::Result& App::executeSolver() const {
    return get<GPU>().solver.execute();
  }

  App::GPU::GPU(wxGLCanvas& canvas) :
      ctx(canvas),
      solver(ctx),
      shader2D("Shaders/2d.vert", "Shaders/2d.frag"),
      shader3D("Shaders/3d.vert", "Shaders/3d.frag"),
      renderer() {
    reloadShaders();
  }

  void App::GPU::reloadShaders() {
    App::get<wxStatusBar>().PushStatusText("Loading shaders...");
    shader2D.load();
    shader3D.load();
    App::get<wxStatusBar>().PopStatusText();
  }
}