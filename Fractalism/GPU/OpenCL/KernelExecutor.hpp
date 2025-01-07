#ifndef _FRACTALISM_KERNEL_EXECUTOR_HPP_
#define _FRACTALISM_KERNEL_EXECUTOR_HPP_

#include <Fractalism/GPU/OpenCL/CLCommon.hpp>

#include <Fractalism/GPU/OpenGL/GLTexture3D.hpp>
#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism::gpu::opencl {
  class KernelExecutor {
  public:
    KernelExecutor(size_t index);
    void updateKernel();
    void updateResolution();
    void updateView();
    void updateParameter();
    bool needsMore() const;
    cl::Event enqueue(std::vector<cl::Event>& waitEvents);
    ViewWindowSettings &settings;
    opengl::GLTexture3D texture;
  private:
    size_t index;
    cl::Kernel kernel;
    std::vector<cl::Memory> clGlTextures;
    cl_uint currentIteration;
  };
}

#endif