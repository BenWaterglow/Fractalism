#ifndef _FRACTALISM_APP_HPP_
#define _FRACTALISM_APP_HPP_

#include <mutex>
#include <optional>
#include <type_traits>
#include <vector>

#include <Fractalism/UI/UICommon.hpp>

#include <Fractalism/GPU/GPU.hpp>
#include <Fractalism/GPU/OpenCL/ProgramManager.hpp>
#include <Fractalism/GPU/OpenGL/GLShaderProgram.hpp>
#include <Fractalism/GPU/GPUContext.hpp>
#include <Fractalism/UI/ViewWindow.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/Settings.hpp>
#include <Fractalism/Exceptions.hpp>

namespace fractalism {

  class App : public wxApp {
  public:
    virtual bool OnInit() override;

    virtual int OnExit() override;

    virtual bool OnExceptionInMainLoop() override;

    virtual void OnUnhandledException() override;

    static void render(std::vector<ui::ViewWindow*>& viewWindow);

    static inline void setGLContext(wxGLCanvas& canvas) {
      get<gpu::GPUContext>().setCurrent(canvas);
    }

    static inline void reloadShaders() {
      get<gpu::GPU>().reloadShaders();
    }

    template<typename Callable, typename... Args>
    static inline std::invoke_result_t<Callable, Args...> doWithStatusMessage(
        const std::string&& message,
        Callable&& callable,
        Args&&... args) {
      wxStatusBar& statusBar = *get<wxFrame>().GetStatusBar();
      statusBar.PushStatusText(message);
      using Return = std::invoke_result_t<Callable, Args...>;
      if constexpr (std::is_void_v<Return>) {
        std::invoke(std::forward<Callable>(callable), std::forward<Args>(args)...);
        statusBar.PopStatusText();
      } else {
        Return value = std::invoke(std::forward<Callable>(callable), std::forward<Args>(args)...);
        statusBar.PopStatusText();
        return value;
      }
    }

    template<typename T>
    static inline std::conditional_t<std::is_same_v<T, gpu::GPUContext>, const T&, T&>
    get() {
      if constexpr (std::is_same_v<T, App>) {
        return *static_cast<App*>(wxApp::GetInstance());
      } else if constexpr (std::is_same_v<T, Settings>) {
        return get<App>().settings;
      } else if constexpr (std::is_same_v<T, wxFrame>) {
        wxFrame* ui = get<App>().ui;
        if (ui == nullptr) {
          throw AssertionError("UI has not been created.");
        }
        return *ui;
      } else if constexpr (std::is_same_v<T, gpu::GPU>) {
        App& app = get<App>();
        if (app.gpu) {
          return *app.gpu;
        } else [[unlikely]] {
          throw AssertionError("GPU manager has not been initialized.");
        }
      } else if constexpr (std::is_same_v<T, gpu::GPUContext>) {
        return get<gpu::GPU>().ctx;
      } else if constexpr (std::is_same_v<T, gpu::opencl::ProgramManager>) {
        return get<gpu::GPU>().kernelManager;
      } else if constexpr (std::is_same_v<T, gpu::opengl::GLShaderProgram>) {
        switch (get<Settings>().renderDimensions) {
        case options::Dimensions::two:   return get<gpu::GPU>().shader2D;
        case options::Dimensions::three: return get<gpu::GPU>().shader3D;
        default: throw AssertionError("Invalid render dimension.");
        }
      } else {
        static_assert(false, "Invalid type requested.");
        return *static_cast<T*>(nullptr);
      }
    }
  private:
    wxFrame* ui = nullptr;
    std::once_flag setupGPU;
    std::optional<gpu::GPU> gpu;
    Settings settings;
  };
}

#endif