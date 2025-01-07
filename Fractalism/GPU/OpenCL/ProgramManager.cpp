#include <Fractalism/GPU/OpenCL/ProgramManager.hpp>

#include <Fractalism/App.hpp>

namespace fractalism::gpu::opencl {
  ProgramManager::ProgramManager(const GPUContext& ctx) : 
        program(App::doWithStatusMessage("Building OpenCL program...", 
          &GPUContext::buildProgram,
          ctx,
          "z = add(sqr(z), c)",
          utils::cayleyDicksonConstruction("complex", "real") +
          utils::cayleyDicksonConstruction("quaternion", "complex") +
          utils::defineMulticomplexNumberSystem("bicomplex", "complex"),
          8.0)),
        svm() {}

  cl::Kernel ProgramManager::findKernel(const std::string& name) const {
    return cl::Kernel(program, name);
  }

  void ProgramManager::createBuffer() {
    svm.addBuffer();
  }

  void ProgramManager::useBuffer(size_t index, std::vector<cl::Event>& waitEvents, cl::Event& doneEvent) {
    return svm.useBuffer(index, waitEvents, doneEvent);
  }

  void ProgramManager::svmKernelArg(cl::Kernel& kernel, cl_uint index) const {
    svm.asKernelArg(kernel, index);
  }

  void ProgramManager::updateResolution() {
    svm.resize(App::get<Settings>().resolution);
  }

  void ProgramManager::freeSvm() {
    svm.free();
  }
}