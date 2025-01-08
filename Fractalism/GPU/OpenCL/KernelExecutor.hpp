#ifndef _FRACTALISM_KERNEL_EXECUTOR_HPP_
#define _FRACTALISM_KERNEL_EXECUTOR_HPP_

#include <Fractalism/GPU/OpenCL/CLCommon.hpp>
#include <Fractalism/GPU/OpenGL/GLTexture3D.hpp>
#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism::gpu::opencl {

/**
 * @class KernelExecutor
 * @brief Manages the execution of OpenCL kernels for fractal rendering.
 */
class KernelExecutor {
public:
  /**
   * @brief Constructs a KernelExecutor for a specific view window.
   * @param index The index of the view window.
   */
  KernelExecutor(size_t index);

  /**
   * @brief Updates the kernel with the latest parameters.
   */
  void updateKernel();

  /**
   * @brief Updates the resolution of the kernel.
   */
  void updateResolution();

  /**
   * @brief Updates the view parameters of the kernel.
   */
  void updateView();

  /**
   * @brief Updates the fractal parameter of the kernel.
   */
  void updateParameter();

  /**
   * @brief Checks if more iterations are needed.
   * @return True if more iterations are needed, false otherwise.
   */
  bool needsMore() const;

  /**
   * @brief Enqueues the kernel for execution.
   * @param waitEvents A vector of events to wait for before executing the
   * kernel.
   * @return An event representing the completion of the kernel execution.
   */
  cl::Event enqueue(std::vector<cl::Event>& waitEvents);

  ViewWindowSettings& settings; ///< Settings for the view window.
  opengl::GLTexture3D texture;  ///< OpenGL texture for rendering.
private:
  size_t index;      ///< Index of the view window.
  cl::Kernel kernel; ///< OpenCL kernel for fractal rendering.
  std::vector<cl::Memory> clGlTextures; ///< OpenCL-OpenGL shared textures.
  cl_uint currentIteration;             ///< Current iteration count.
};
} // namespace fractalism::gpu::opencl

#endif
