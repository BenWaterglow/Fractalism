#include <memory>
#include <vector>
#include <GL/glew.h>
#include "CLIncludeHelper.hpp"
#include <CL/cl_gl.h>

#include "CLSolver.hpp"
#include "Utils.hpp"
#include "Exceptions.hpp"
#include "GLTexture3D.hpp"

#define multicomplexFunctions

static auto defineMulticomplexNumberSystem(const char* numberSystem, const char* elementSystem) {
  return utils::format("X(%s, %s, "
    "(sub_%s(mul_%s(x.x, y.x), mul_%s(x.y, y.y)), add_%s(mul_%s(x.x, y.y), mul_%s(x.y, y.x))), "
    "(sub_%s(sqr_%s(x.x), sqr_%s(x.y)), scale_%s(mul_%s(x.x, x.y), 2.0)), "
    "modulus_sq_%s(x.x) + modulus_sq_%s(x.y)"
    ")",
    numberSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem,
    elementSystem);
}

CLSolver::CLSolver(GPUContext& ctx, wxStatusBar& statusBar) :
  ctx(ctx),
  program((statusBar.PushStatusText("Creating OpenCL solver program..."), ctx.buildProgram(
    "z = add(sqr(z), c)",
    utils::format("%s %s %s",
      defineMulticomplexNumberSystem("complex", "real").get(),
      defineMulticomplexNumberSystem("bicomplex", "complex").get(),
      defineMulticomplexNumberSystem("tricomplex", "bicomplex").get()).get(),
    8,
    8.0))),
  kernels(program) {
  statusBar.PopStatusText();
}

CLSolver::~CLSolver() {}

CLSolver::Output::Output(cl::NDRange& range) : texture(range[0], range[1], range[2]) {}

CLSolver::Program* CLSolver::createProgram(options::Space space) {
  return new Program(*this, kernels.get(space));
}

CLSolver::Program::Program(CLSolver& solver, Kernels::Space& spaceKernels) :
    solver(solver),
    spaceKernels(spaceKernels),
    task(nullptr) {}

CLSolver::Program::~Program() {
  delete task;
}

CLSolver::Program& CLSolver::Program::operator=(Program& rhs) {
  UNUSED(rhs);
  throw std::runtime_error("Operation not implemented");
}

bool CLSolver::Program::isPrepared() {
  return task != nullptr;
}

void CLSolver::Program::prepare(options::Dimensions dimensions, int resolution) {
  delete task;
  task = new Task(spaceKernels, solver.ctx, dimensions, resolution);
}

void CLSolver::Program::setNumberSystem(options::NumberSystem numberSystem) {
  task->setNumberSystem(solver.program, numberSystem);
}

void CLSolver::Program::setParameters(int maxIterations, int iterationsPerFrame, cltypes::Viewspace view, cltypes::Number parameter) {
  task->setParameters(maxIterations, iterationsPerFrame, view, parameter);
}

CLSolver::Output& CLSolver::Program::execute() {
  return task->execute(solver.ctx.queue);
}

CLSolver::Task::Task(Kernels::Space& spaceKernels, GPUContext& ctx, options::Dimensions dimensions, int resolution) :
  spaceKernels(spaceKernels),
  kernel(nullptr),
  range(dimensions == options::Dimensions::three ?
    cl::NDRange(resolution, resolution, resolution) :
    cl::NDRange(resolution, resolution)),
  buffer(ctx, range),
  output(range),
  clGlTextures(std::vector<cl::Memory>{ output.texture(ctx.clCtx) }),
  view(),
  parameter(),
  maxIterations(),
  currentIteration(),
  iterationsPerFrame() {}

template <typename T>
void CLSolver::Task::setKernelArg(cl::Kernel& kernel, cl_uint idx, T& arg) {
  kernel.setArg(idx, arg);
}

void CLSolver::Task::setKernelArg(cl::Kernel& kernel, cl_uint idx, CLSolver::SVMPtrArray<cltypes::WorkStore>& arg) {
  arg.setKernelArg(kernel, idx);
}

// Termination point for parameter packing
template <typename T>
void CLSolver::Task::setKernelParameters(cl::Kernel& kernel, cl_uint idx, T& arg) {
  setKernelArg(kernel, idx, arg);
}

template <typename T, typename... P>
void CLSolver::Task::setKernelParameters(cl::Kernel& kernel, cl_uint idx, T& arg, P&... moreArgs) {
  setKernelArg(kernel, idx, arg);
  setKernelParameters(kernel, idx + 1, moreArgs...);
}

CLSolver::Task::~Task() {}

CLSolver::Task& CLSolver::Task::operator=(Task& rhs) {
  UNUSED(rhs);
  throw std::runtime_error("Operation not implemented");
}

CLSolver::Output& CLSolver::Task::execute(cl::CommandQueue& queue) {
  if (currentIteration < maxIterations && kernel) {
    cl_uint maxIterationThisFrame = currentIteration + iterationsPerFrame;
    if (maxIterationThisFrame > maxIterations) {
      maxIterationThisFrame = maxIterations;
    }

    try {
      output.texture.clear();
      setKernelParameters(
        *kernel,
        2,
        view,
        parameter,
        currentIteration,
        maxIterationThisFrame);

      glFinish();
      queue.enqueueAcquireGLObjects(&clGlTextures);
      queue.enqueueNDRangeKernel(
        *kernel,
        cl::NullRange,
        range);
      queue.enqueueReleaseGLObjects(&clGlTextures);
      queue.finish();
    } catch (const cl::Error& e) {
      throw CLError("Could not execute kernel", e);
    }
    currentIteration = maxIterationThisFrame;
  }
  return output;
}

void CLSolver::Task::setNumberSystem(cl::Program& program, options::NumberSystem numberSystem) {
  Kernels::Space::RenderMode& renderModeKernels = spaceKernels.escape;
  kernel = &renderModeKernels.get(numberSystem);

  setKernelParameters(*kernel, 0, clGlTextures[0], buffer);
}

void CLSolver::Task::setParameters(int maxIterations, int iterationsPerFrame, cltypes::Viewspace view, cltypes::Number parameter) {
  currentIteration = 0;
  this->maxIterations = maxIterations;
  this->iterationsPerFrame = iterationsPerFrame;
  this->view = view;
  this->parameter = parameter;
}

/******************************************************
 * OpenCL SVM management
 ******************************************************/

template <typename T>
CLSolver::SVMPtrArray<T>::SVMPtrArray(cl::Context& ctx, cl::CommandQueue& queue, size_t maxMemAllocSize, size_t itemCount, cl_svm_mem_flags flags, cl_uint alignment) :
    ptr(ctx, flags, ((itemCount * sizeof(T)) / maxMemAllocSize) + 1, alignment) {
  size_t maxBufferItemCount = maxMemAllocSize / sizeof(T);

  if (!ptr) {
    throw CLSVMAllocationError("Could not allocate main SVM buffer");
  }
  std::vector<SVMPtr<T>> subBuffers;
  subBuffers.reserve(ptr.itemCount);
  for (size_t i = 0; i < ptr.itemCount - 1; i++) {
    subBuffers.emplace_back(ctx, flags, maxBufferItemCount, alignment);
    if (!subBuffers[i]) {
      throw CLSVMAllocationError("Could not allocate SVM buffer #%d", i);
    }
  }
  subBuffers.emplace_back(ctx, flags, itemCount % maxBufferItemCount, alignment);
  if (!subBuffers[ptr.itemCount - 1]) {
    throw CLSVMAllocationError("Could not allocate SVM buffer #%d", ptr.itemCount - 1);
  }

  cl_int err = clEnqueueSVMMap(queue(), CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, ptr, ptr.size(), 0, nullptr, nullptr);
  if (err) {
    throw CLError("Could not map OpenCL SVM buffer", err);
  }

  for (size_t i = 0; i < ptr.itemCount; i++) {
    // Don't need to keep the individual wrapper objects long-term.
    // They're just used to use RAII to make this exception-safe.
    ptr[i] = subBuffers[i].claim();
  }
  err = clEnqueueSVMUnmap(queue(), ptr, 0, nullptr, nullptr);
  if (err) {
    throw CLError("Could not unmap OpenCL SVM buffer", err);
  }
}

template <typename T>
CLSolver::SVMPtrArray<T>::SVMPtrArray(GPUContext& ctx, cl::NDRange& range, cl_svm_mem_flags flags, cl_uint alignment) : 
  SVMPtrArray(ctx.clCtx, ctx.queue, ctx.maxMemAllocSize, range[0] * range[1] * range[2], flags, alignment) {}

template <typename T>
CLSolver::SVMPtrArray<T>::~SVMPtrArray() {
  for (size_t i = 0; i < ptr.itemCount; i++) {
    // Manually free, since we explicity claimed ownership
    clSVMFree(ptr.ctx(), ptr[i]);
  }
}

template <typename T>
void CLSolver::SVMPtrArray<T>::setKernelArg(cl::Kernel& kernel, cl_uint idx) {
  cl_int err = clSetKernelArgSVMPointer(kernel(), idx, ptr);
  if (err) {
    throw CLError("Could not set OpenCL SVM pointer kernel argument", err);
  }
  err = clSetKernelExecInfo(kernel(), CL_KERNEL_EXEC_INFO_SVM_PTRS, ptr.size(), ptr);
  if (err) {
    throw CLError("Could not set OpenCL kernel exec info CL_KERNEL_EXEC_INFO_SVM_PTRS", err);
  }
}

/******************************************************
 * Kernel management
 ******************************************************/

template<utils::String kernelName>
inline cl::Kernel CLSolver::Kernels::create(const cl::Program& program) {
  return cl::Kernel(program, kernelName);
}

inline CLSolver::Kernels::Kernels(const cl::Program& program) :
  phase(Space::create<"phase"_s>(program)),
  dynamical(Space::create<"dynamical"_s>(program)) {
}

template<utils::String prefix>
inline CLSolver::Kernels::Space::RenderMode CLSolver::Kernels::Space::RenderMode::create(const cl::Program& program) {
  return {
    Kernels::create<prefix + "_complex">(program),
    Kernels::create<prefix + "_bicomplex">(program),
    Kernels::create<prefix + "_tricomplex">(program)
  };
}

template<utils::String prefix>
inline CLSolver::Kernels::Space CLSolver::Kernels::Space::create(const cl::Program& program) {
  return {
    RenderMode::create<prefix + "_escape">(program),
    RenderMode::create<prefix + "_translated">(program)
  };
}

CLSolver::Kernels::Space& CLSolver::Kernels::get(options::Space space) {
  switch (space) {
  case options::Space::phase:
    return phase;
  case options::Space::dynamical:
    return dynamical;
  default:
    throw std::invalid_argument("Invalid computation space");
  }
}

CLSolver::Kernels::Space::RenderMode& CLSolver::Kernels::Space::get(options::RenderMode renderMode) {
  switch (renderMode) {
  case options::RenderMode::escape:
    return escape;
  case options::RenderMode::translated:
    return translated;
  default:
    throw std::invalid_argument("Invalid render mode");
  }
}

cl::Kernel& CLSolver::Kernels::Space::RenderMode::get(options::NumberSystem numberSystem) {
  switch (numberSystem) {
  case options::NumberSystem::c1:
    return c1;
  case options::NumberSystem::c2:
    return c2;
  case options::NumberSystem::c3:
    return c3;
  default:
    throw std::invalid_argument("Invalid number system");
  }
}
