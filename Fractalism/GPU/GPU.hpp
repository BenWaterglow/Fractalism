#ifndef _FRACTALISM_GPU_HPP_
#define _FRACTALISM_GPU_HPP_

#include <Fractalism/UI/UICommon.hpp>

// we don't use glew in this header, but it must be included before gl.h
#include <GL/glew.h>
#include <wx/glcanvas.h>

#include <Fractalism/GPU/GPUContext.hpp>
#include <Fractalism/GPU/OpenCL/ProgramManager.hpp>
#include <Fractalism/GPU/OpenGL/GLRenderer.hpp>
#include <Fractalism/GPU/OpenGL/GLShaderProgram.hpp>

namespace fractalism::gpu {

/**
 * @struct GPU
 * @brief Manages the GPU context, shaders, and rendering for the Fractalism
 * application.
 */
struct GPU {
  /**
   * @brief Constructs a GPU object with the specified OpenGL canvas.
   * @param canvas The OpenGL canvas.
   */
  GPU(wxGLCanvas &canvas);

  /**
   * @brief Reloads the shaders.
   */
  void reloadShaders();

  const GPUContext ctx;                 ///< The GPU context.
  opengl::GLShaderProgram shader2D;     ///< The 2D shader program.
  opengl::GLShaderProgram shader3D;     ///< The 3D shader program.
  opengl::GLRenderer renderer;          ///< The OpenGL renderer.
  opencl::ProgramManager kernelManager; ///< The OpenCL program manager.
};
} // namespace fractalism::gpu

#endif
