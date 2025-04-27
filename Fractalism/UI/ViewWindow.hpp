#ifndef _FRACTALISM_VIEW_WINDOW_HPP_
#define _FRACTALISM_VIEW_WINDOW_HPP_

#include <Fractalism/GPU/OpenCL/KernelExecutor.hpp>
#include <Fractalism/UI/Controls/IterationToolBar.hpp>
#include <Fractalism/UI/Controls/ViewspaceToolBar.hpp>
#include <Fractalism/UI/GLRenderCanvas.hpp>
#include <Fractalism/UI/UICommon.hpp>
#include <vector>
#include <wx/aui/framemanager.h>

namespace fractalism::ui {

/**
 * @class ViewWindow
 * @brief A window for displaying and interacting with a fractal view.
 */
class ViewWindow : public wxPanel {
public:
  /**
   * @brief Constructs a ViewWindow.
   * @param parent The parent window.
   * @param index The index of the view window.
   */
  ViewWindow(wxWindow& parent, size_t index);

  /**
   * @brief Implicit conversion to wxGLCanvas.
   * @return The OpenGL render canvas.
   */
  inline operator wxGLCanvas& () const { return renderCanvas; }

  /**
   * @brief Initializes the view window.
   */
  void init();

  /**
   * @brief Clears the texture if necessary.
   */
  void maybeClearTexture();

  /**
   * @brief Enqueues a render operation.
   * @param waitEvents A vector of events to wait for before rendering.
   */
  void enqueueRender(std::vector<cl::Event>& waitEvents);

  /**
   * @brief Updates the fractal parameter.
   */
  void updateParameter();

  /**
   * @brief Updates the view.
   * 
   * Eqivalent to @code
   * updateCenter();
   * updateViewMapping();
   * updateZoom();
   * @endcode
   */
  void updateView();

  /**
   * @brief Updates the center of the view.
   */
  void updateCenter();

  /**
   * @brief Updates the view mapping.
   */
  void updateViewMapping();

  /**
   * @brief Updates the zoom level.
   */
  void updateZoom();

  /**
   * @brief Updates the number system.
   */
  void updateNumberSystem();

  /**
   * @brief Updates the render dimensions.
   */
  void updateRenderDimensions();

  /**
   * @brief Updates the iteration modifier.
   */
  void updateIterationModifier();

  /**
   * @brief Updates the iterations per frame.
   */
  void updateIterationsPerFrame();

  /**
   * @brief Returns the ViewWindowSettings for this window.
   * @return The ViewWindowSettings for this window.
   */
  inline ViewWindowSettings& getSettings() {
    return kernel.settings;
  }

private:
  gpu::opencl::KernelExecutor kernel;           ///< The OpenCL kernel executor.
  wxAuiManager auiManager;                      ///< Manager for the frame layout.
  wxStatusBar& statusBar;                       ///< The status bar to update.
  GLRenderCanvas& renderCanvas;                 ///< The OpenGL render canvas.
  controls::ViewspaceToolBar& viewspaceToolBar; ///< Toolbar for controlling the viewspace.
  controls::IterationToolBar& iterationToolBar; ///< Toolbar for controlling iterations.
  
  /**
   * @brief Fired whenever the Viewspace is changed.
   */
  void onViewChanged();
};
} // namespace fractalism::ui

#endif
