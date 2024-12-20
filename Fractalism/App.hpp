#ifndef _FRACTALISM_APP_HPP_
#define _FRACTALISM_APP_HPP_

#include <cassert>
#include <type_traits>

#include <Fractalism/UI/WxIncludeHelper.hpp>

#include <Fractalism/GPU/GPU.hpp>
#include <Fractalism/GPU/OpenCL/CLSolver.hpp>
#include <Fractalism/GPU/OpenGL/GLShaderProgram.hpp>
#include <Fractalism/GPU/GPUContext.hpp>
#include <Fractalism/GPU/OpenGL/GLRenderer.hpp>
#include <Fractalism/UI/UI.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/Settings.hpp>
#include <Fractalism/Exceptions.hpp>

namespace fractalism {

  class App : public wxApp {
  private:
    using GPU = gpu::GPU;
    using CLSolver = gpu::opencl::CLSolver;
    using GLShaderProgram = gpu::opengl::GLShaderProgram;
    using GLRenderer = gpu::opengl::GLRenderer;
    using UI = ui::UI;
    UI* ui;
    GPU* gpu;
    Settings settings;
  public:
    App();
    ~App();
    virtual bool OnInit() override;
    void setupGPUContext(wxGLCanvas& canvas);
    CLSolver::Result& executeSolver() const;

    static inline void setGLContext(wxGLCanvas& canvas) {
      App::get<GPU>().ctx.glCtx.SetCurrent(canvas);
    }

    static inline void reloadShaders() {
      App::get<GPU>().reloadShaders();
    }

    template<typename T>
    static inline T& get() {
      if constexpr (std::is_same_v<T, App>) {
        return *static_cast<App*>(wxApp::GetInstance());
      } else if constexpr (std::is_same_v<T, Settings>) {
        return get<App>().settings;
      } else if constexpr (std::is_same_v<T, UI>) {
        UI* ui = get<App>().ui;
        if (ui == nullptr) {
          throw FractalismError("");
        }
        return *ui;
      } else if constexpr (std::is_same_v<T, wxStatusBar>) {
        return *get<UI>().GetStatusBar();
      } else if constexpr (std::is_same_v<T, GPU>) {
        GPU* gpu = get<App>().gpu;
        assert(("No valid GPU context exists.", gpu));
        return *gpu;
      } else if constexpr (std::is_same_v<T, CLSolver>) {
        return get<GPU>().solver;
      } else if constexpr (std::is_same_v<T, GLShaderProgram>) {
        switch (get<Settings>().renderDimensions) {
        case options::Dimensions::two:   return get<GPU>().shader2D;
        case options::Dimensions::three: return get<GPU>().shader3D;
        default: assert(("Invalid render dimension.", false)); return *static_cast<T*>(nullptr);
        }
      } else if constexpr (std::is_same_v<T, GLRenderer>) {
        return get<GPU>().renderer;
      } else {
        static_assert(false, "Invalid type requested.");
        return *static_cast<T*>(nullptr);
      }
    }
  };
}

#endif