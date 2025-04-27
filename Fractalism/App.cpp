#include <Fractalism/App.hpp>
#include <Fractalism/UI/UI.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>

wxIMPLEMENT_APP(fractalism::App);

namespace fractalism {

  bool App::OnInit() {
    ui = new ui::UI();
    ui->Show(true);
    return true;
  }

  int App::OnExit() {
    // When this method is called, the static instance is already cleared,
    // but we need it to free the SVM.
    wxAppConsole* instance = wxApp::GetInstance();
    wxApp::SetInstance(this);
    if (gpu) {
      gpu->kernelManager.freeSvm();
    }
    wxApp::SetInstance(instance);
    return wxApp::OnExit();
  }

  bool App::OnExceptionInMainLoop() {
    try {
      throw;
    } catch (const std::exception& e) {
      wxLogError("Unexpected error: %s", e.what());
    }
    return false;
  }

  void App::OnUnhandledException() {
    try {
      throw;
    } catch (const std::exception &e) {
      wxLogFatalError("Unhndled exception: %s", e.what());
    }
  }

  void App::render(std::vector<ui::ViewWindow*>& viewWindows) {
    if (viewWindows.size()) {
      std::call_once(App::get<App>().setupGPU, [](std::vector<ui::ViewWindow*>& viewWindows) {
        std::optional<gpu::GPU>& gpu = App::get<App>().gpu;
        for (ui::ViewWindow* viewWindow : viewWindows) {
          if (viewWindow->IsShownOnScreen()) {
            if (!gpu) {
              gpu.emplace(*viewWindow);
              App::get<gpu::opencl::ProgramManager>().updateResolution();
              break;
            }
          }
        }
        if (!gpu) {
          throw AssertionError("Could not initialize GPU context.");
        }
        for (ui::ViewWindow* viewWindow : viewWindows) {
          gpu->kernelManager.createBuffer();
          viewWindow->init();
        }
      }, viewWindows);
      
      try {
        for (ui::ViewWindow *viewWindow : viewWindows) {
          viewWindow->maybeClearTexture();
        }

        std::vector<cl::Event> waitEvents{}; // Not used yet.
        for (ui::ViewWindow *viewWindow : viewWindows) {
          viewWindow->enqueueRender(waitEvents);
        }
        App::get<gpu::GPUContext>().queue.finish();
      } catch (const std::exception &e) {
        wxSafeShowMessage("Error", e.what());
        std::abort();
      }
    }
  }
}