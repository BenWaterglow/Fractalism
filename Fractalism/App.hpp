#ifndef _FRACTALISM_APP_HPP_
#define _FRACTALISM_APP_HPP_

#include <mutex>
#include <optional>
#include <type_traits>
#include <vector>

#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/GPU.hpp>
#include <Fractalism/GPU/GPUContext.hpp>
#include <Fractalism/GPU/OpenCL/ProgramManager.hpp>
#include <Fractalism/GPU/OpenGL/GLShaderProgram.hpp>
#include <Fractalism/Options.hpp>
#include <Fractalism/Settings.hpp>
#include <Fractalism/UI/UICommon.hpp>
#include <Fractalism/UI/ViewWindow.hpp>

namespace fractalism {

/**
 * @class App
 * @brief The main application class for Fractalism.
 */
class App : public wxApp {
public:
  /**
   * @brief Initializes the application.
   * @return True if initialization is successful, false otherwise.
   */
  virtual bool OnInit() override;

  /**
   * @brief Cleans up the application on exit.
   * @return The exit code.
   */
  virtual int OnExit() override;

  /**
   * @brief Handles exceptions in the main loop.
   * @return True if the exception is handled, false otherwise.
   */
  virtual bool OnExceptionInMainLoop() override;

  /**
   * @brief Handles unhandled exceptions.
   */
  virtual void OnUnhandledException() override;

  /**
   * @brief Renders the view windows.
   * @param viewWindow The vector of view windows to render.
   */
  static void render(std::vector<ui::ViewWindow*>& viewWindow);

  /**
   * @brief Sets the current OpenGL context.
   * @param canvas The OpenGL canvas.
   */
  static inline void setGLContext(wxGLCanvas& canvas) {
    get<gpu::GPUContext>().setCurrent(canvas);
  }

  /**
   * @brief Reloads the shaders.
   */
  static inline void reloadShaders() { get<gpu::GPU>().reloadShaders(); }

  /**
   * @brief Executes a callable with a status message.
   * @tparam Callable The type of the callable.
   * @tparam Args The types of the arguments to the callable.
   * @param message The status message to display.
   * @param callable The callable to execute.
   * @param args The arguments to pass to the callable.
   * @return The result of the callable.
   */
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

  /**
   * @brief Gets a reference to a specific type.
   * @tparam T The type to get.
   * @return A reference to the requested type.
   */
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
      case options::Dimensions::two:
        return get<gpu::GPU>().shader2D;
      case options::Dimensions::three:
        return get<gpu::GPU>().shader3D;
      default:
        throw AssertionError("Invalid render dimension.");
      }
    } else {
      static_assert(false, "Invalid type requested.");
      return *static_cast<T*>(nullptr);
    }
  }

private:
  wxFrame* ui = nullptr;       ///< The main UI frame.
  std::once_flag setupGPU;     ///< Flag to ensure GPU setup is done once.
  std::optional<gpu::GPU> gpu; ///< The GPU manager.
  Settings settings;           ///< The application settings.
};
} // namespace fractalism

#endif
