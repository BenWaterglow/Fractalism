#ifndef _FRACTALISM_PROGRAM_MANAGER_HPP_
#define _FRACTALISM_PROGRAM_MANAGER_HPP_

#include <Fractalism/GPU/GPUContext.hpp>
#include <Fractalism/GPU/OpenCL/KernelExecutor.hpp>
#include <Fractalism/GPU/OpenCL/SVMPtr.hpp>
#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/ViewWindowSettings.hpp>
#include <string>
#include <vector>

namespace fractalism::gpu::opencl {

/**
 * @class ProgramManager
 * @brief Manages OpenCL programs and buffers for fractal rendering.
 */
class ProgramManager {
public:
  /**
   * @brief Constructs a ProgramManager with the given GPU context.
   * @param ctx The GPU context.
   */
  ProgramManager(const GPUContext& ctx);

  /**
   * @brief Finds an OpenCL kernel by name.
   * @param name The name of the kernel.
   * @return The OpenCL kernel.
   */
  cl::Kernel findKernel(const std::string& name) const;

  /**
   * @brief Creates a buffer for the program.
   */
  void createBuffer();

  /**
   * @brief Uses a buffer for the program.
   * @param index The index of the buffer.
   * @param waitEvents A vector of events to wait for before using the buffer.
   * @param doneEvent The event to signal when the buffer is ready.
   */
  void useBuffer(
      size_t index,
      std::vector<cl::Event>& waitEvents,
      cl::Event& doneEvent);

  /**
   * @brief Sets a kernel argument to use the SVM buffer.
   * @param kernel The kernel to set the argument for.
   * @param index The index of the argument.
   */
  void svmKernelArg(cl::Kernel& kernel, cl_uint index) const;

  /**
   * @brief Updates the resolution of the program.
   */
  void updateResolution();

  /**
   * @brief Frees the SVM buffer.
   */
  void freeSvm();

private:
  cl::Program program;                           ///< The OpenCL program.
  BackBufferedSvmArrayPtr<types::WorkStore> svm; ///< The SVM buffer.
};
} // namespace fractalism::gpu::opencl

#endif
