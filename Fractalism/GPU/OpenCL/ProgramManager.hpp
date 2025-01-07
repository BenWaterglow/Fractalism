#ifndef _FRACTALISM_KERNEL_EXECUTOR_MANAGER_HPP_
#define _FRACTALISM_KERNEL_EXECUTOR_MANAGER_HPP_

#include <string>
#include <vector>
#include <Fractalism/GPU/Types.hpp>
#include <Fractalism/GPU/GPUContext.hpp>
#include <Fractalism/GPU/OpenCL/SVMPtr.hpp>
#include <Fractalism/GPU/OpenCL/KernelExecutor.hpp>
#include <Fractalism/ViewWindowSettings.hpp>

namespace fractalism::gpu::opencl {
  class ProgramManager {
  public:
    ProgramManager(const GPUContext& ctx);
    cl::Kernel findKernel(const std::string& name) const;
    void createBuffer();
    void useBuffer(size_t index, std::vector<cl::Event>& waitEvents, cl::Event& doneEvent);
    void svmKernelArg(cl::Kernel& kernel, cl_uint index) const;
    void updateResolution();
    void freeSvm();
  private:
    cl::Program program;
    BackBufferedSvmArrayPtr<types::WorkStore> svm;
  };
}

#endif