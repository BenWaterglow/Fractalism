#ifndef _FRACTALISM_GPU_CONTEXT_HPP_
#define _FRACTALISM_GPU_CONTEXT_HPP_

#include <string>

#include <Fractalism/GPU/OpenCL/CLCommon.hpp>
#include <Fractalism/UI/UICommon.hpp>

// we don't use glew in this header, but it must be included before gl.h
#include <GL/glew.h>
#include <wx/glcanvas.h>

namespace fractalism::gpu {

/**
 * @struct GPUContext
 * @brief Manages the GPU context for OpenCL and OpenGL operations.
 */
struct GPUContext {
  /**
   * @brief Constructs a GPUContext with the specified OpenGL canvas.
   * @param canvas The OpenGL canvas.
   */
  GPUContext(wxGLCanvas &canvas);

  /**
   * @brief Builds an OpenCL program with the specified parameters.
   * @param function The mathematical function to build the kernel around.
   * @param numberSystemDefinitions The number system definitions.
   * @param escapeValue The escape value for the fractal computation.
   * @return The built OpenCL program.
   */
  cl::Program buildProgram(
      const std::string&& function,
      const std::string&& numberSystemDefinitions,
      double escapeValue) const;

  /**
   * @brief Sets the current OpenGL context to the specified canvas.
   * @param canvas The OpenGL canvas.
   */
  inline void setCurrent(wxGLCanvas& canvas) const { canvas.SetCurrent(glCtx); }

  /**
   * @brief Implicit conversion to const cl::Context&.
   * @return The OpenCL context.
   */
  inline operator const cl::Context& () const { return clCtx; }

  /**
   * @brief Implicit conversion to const cl_context&.
   * @return The OpenCL context.
   */
  inline operator const cl_context& () const { return clCtx(); }

  cl::Device device;            ///< OpenCL device used for computation.
  cl::Context clCtx;            ///< OpenCL context for device communication.
  cl::CommandQueue queue;       ///< Queue to manage OpenCL command execution.
  cl_ulong maxMemAllocSize = 0; ///< Maximum memory allocatable on the device.

private:
  wxGLContext glCtx; ///< OpenGL context for device communication.
};
} // namespace fractalism::gpu

#endif
