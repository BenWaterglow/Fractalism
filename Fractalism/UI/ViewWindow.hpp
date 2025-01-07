#ifndef _FRACTALISM_VIEW_WINDOW_HPP_
#define _FRACTALISM_VIEW_WINDOW_HPP_

#include <vector>
#include <Fractalism/UI/UICommon.hpp>
#include <Fractalism/UI/GLRenderCanvas.hpp>
#include <Fractalism/UI/Controls/ViewspaceToolBar.hpp>
#include <Fractalism/UI/Controls/IterationToolBar.hpp>
#include <Fractalism/GPU/OpenCL/KernelExecutor.hpp>
#include <wx/aui/framemanager.h>

namespace fractalism::ui {
  class ViewWindow : public wxPanel {
  public:
    ViewWindow(wxWindow& parent, size_t index);
    inline operator wxGLCanvas& () const {
      return renderCanvas;
    }
    void init();
    void maybeClearTexture();
    void enqueueRender(std::vector<cl::Event>& waitEvents);
    void updateParameter();
    void updateCenter();
    void updateViewMapping();
    void updateZoom();
    void updateNumberSystem();
    void updateRenderDimensions();
    void updateIterationModifier();
    void updateIterationsPerFrame();
  private:
    gpu::opencl::KernelExecutor kernel;
    wxAuiManager auiManager;
    wxStatusBar& statusBar;
    GLRenderCanvas& renderCanvas;
    controls::ViewspaceToolBar& viewspaceToolBar;
    controls::IterationToolBar& iterationToolBar;
  };
}

#endif