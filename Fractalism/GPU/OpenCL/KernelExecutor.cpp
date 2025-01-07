#include <Fractalism/GPU/OpenCL/KernelExecutor.hpp>
#include <Fractalism/GPU/OpenCL/ProgramManager.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/App.hpp>

namespace fractalism::gpu::opencl {
  namespace KernelArg {
    enum KernelArg : cl_uint {
      output,
      buffer,
      view,
      parameter,
      lastIteration,
      maxIterations
    };
  }

  KernelExecutor::KernelExecutor(size_t index) :
        index(index),
        settings(App::get<Settings>().viewWindowSettings[index]),
        kernel(),
        texture(),
        clGlTextures(),
        currentIteration(0) {}

  void KernelExecutor::updateKernel() {
    kernel = App::get<ProgramManager>().findKernel(
      options::kernelName(
        settings.space,
        settings.renderMode,
        App::get<Settings>().numberSystem));
    updateResolution();
    updateParameter();
  }

  void KernelExecutor::updateResolution() {
    clGlTextures.clear();
    texture.resize(App::get<Settings>().resolution);
    clGlTextures = {texture};
    kernel.setArg(KernelArg::output, clGlTextures[0]);
    App::get<ProgramManager>().svmKernelArg(kernel, KernelArg::buffer);
    updateView();
  }

  void KernelExecutor::updateView() {
    settings.view.asKernelArg(kernel, KernelArg::view);
    currentIteration = 0;
  }

  void KernelExecutor::updateParameter() {
    App::get<Settings>().parameter.asKernelArg(kernel, KernelArg::parameter);
    if (settings.space == options::Space::dynamical) {
      currentIteration = 0;
    }
  }

  bool KernelExecutor::needsMore() const {
    return currentIteration < settings.getMaxIterations();
  }

  cl::Event KernelExecutor::enqueue(std::vector<cl::Event>& waitEvents) {
    cl_uint iterationsPerFrame = settings.getIterationsPerFrame();
    cl_uint maxIterationsThisFrame = std::min(currentIteration + iterationsPerFrame, settings.getMaxIterations());
    kernel.setArg(KernelArg::lastIteration, currentIteration);
    kernel.setArg(KernelArg::maxIterations, maxIterationsThisFrame);

    std::vector<cl::Event> bufferDoneEvent{cl::Event()};
    App::get<ProgramManager>().useBuffer(index, waitEvents, bufferDoneEvent[0]);
    const cl::CommandQueue& queue = App::get<GPUContext>().queue;
    std::vector<cl::Event> glObjectsAcquired{cl::Event()};
    queue.enqueueAcquireGLObjects(&clGlTextures, &bufferDoneEvent, glObjectsAcquired.data());
    std::vector<cl::Event> kernelDone{cl::Event()};
    queue.enqueueNDRangeKernel(
      kernel,
      cl::NullRange,
      App::get<Settings>().resolution,
      cl::NullRange,
      &glObjectsAcquired,
      kernelDone.data());
    cl::Event releaseDone;
    queue.enqueueReleaseGLObjects(&clGlTextures, &kernelDone, &releaseDone);
    currentIteration = maxIterationsThisFrame;
    return releaseDone;
  }
}