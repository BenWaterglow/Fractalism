#ifndef _FRACTALISM_GL_RENDERER_HPP_
#define _FRACTALISM_GL_RENDERER_HPP_

#include <Fractalism/UI/UICommon.hpp>
#include <Fractalism/ViewWindowSettings.hpp>
#include <GL/glew.h>
#include <wx/glcanvas.h>

namespace fractalism::gpu::opengl {

/**
 * @class GLRenderer
 * @brief Manages OpenGL rendering for the Fractalism application.
 */
class GLRenderer {
public:
  /**
   * @brief Constructs a GLRenderer.
   */
  GLRenderer();

  /**
   * @brief Destructor that cleans up OpenGL resources.
   */
  ~GLRenderer();

  /**
   * @brief Renders the view using the specified settings and canvas.
   * @param settings The settings for the view window.
   * @param canvas The OpenGL canvas to render to.
   * @param texture The OpenGL texture to use for rendering.
   */
  void render(ViewWindowSettings& settings, wxGLCanvas& canvas, GLuint texture) const;

private:
  GLuint VBOs[4]; ///< Vertex Buffer Objects for rendering.
  GLuint VAOs[2]; ///< Vertex Array Objects for rendering.
};
} // namespace fractalism::gpu::opengl

#endif
