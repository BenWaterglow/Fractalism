#ifndef _FRACTALISM_GL_RENDER_CANVAS_HPP_
#define _FRACTALISM_GL_RENDER_CANVAS_HPP_

#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/UI/UICommon.hpp>
#include <Fractalism/ViewWindowSettings.hpp>
#include <GL/glew.h>
#include <wx/glcanvas.h>

namespace fractalism::ui {

/**
 * @class GLRenderCanvas
 * @brief A canvas for rendering OpenGL content.
 */
class GLRenderCanvas : public wxGLCanvas {
public:
  /**
   * @brief Constructs a GLRenderCanvas.
   * @param parent The parent window.
   * @param settings The view window settings.
   * @param statusBar The status bar to update.
   */
  GLRenderCanvas(
      wxWindow& parent,
      ViewWindowSettings& settings,
      wxStatusBar& statusBar);

private:
  gpu::types::Coordinates lastPoint; ///< The last point hovered over.

  /**
   * @brief Sets the last point hovered over.
   * @param coordinates The coordinates of the last point.
   */
  void setLastPoint(const gpu::types::Coordinates& coordinates);
};
} // namespace fractalism::ui

#endif
