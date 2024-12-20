#include <memory>
#include <utility>
#include <format>
#include <GL/glew.h>

#include <Fractalism/App.hpp>
#include <Fractalism/GPU/OpenCL/CLSolver.hpp>
#include <Fractalism/Utils.hpp>
#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenGL/GLTexture3D.hpp>

namespace fractalism {
  namespace gpu {
    namespace opencl {
      static std::string defineMulticomplexNumberSystem(std::string&& numberSystem, std::string&& elementSystem) {
        return std::format("X("
          /* number_system  */ "{0},"
          /* element_system */ "{1},"
          /* conj           */ "(a, b)," // not actually accurate. There are n unique, valid conjucates for Cn.
          /* mul            */ "(sub_{1}(mul_{1}(a, c), mul_{1}(d, b)), add_{1}(mul_{1}(d, a), mul_{1}(b, c))),"
          /* sqr            */ "(sub_{1}(sqr_{1}(a), sqr_{1}(b)), scale_{1}(mul_{1}(a, b), 2.0)),"
          /* modulus_sq     */ "(modulus_sq_{1}(a) + modulus_sq_{1}(b)))",
          numberSystem,
          elementSystem);
      }

      static std::string cayleyDicksonConstruction(std::string&& numberSystem, std::string&& elementSystem) {
        return std::format("X("
          /* number_system  */ "{0},"
          /* element_system */ "{1},"
          /* conj           */ "(conj_{1}(a), neg_{1}(b)),"
          /* mul            */ "(sub_{1}(mul_{1}(a, c), mul_{1}(conj_{1}(d), b)), add_{1}(mul_{1}(d, a), mul_{1}(b, conj_{1}(c)))),"
          /* sqr            */ "(sub_{1}(sqr_{1}(a), mul_{1}(conj_{1}(b), b)), add_{1}(mul_{1}(b, a), mul_{1}(b, conj_{1}(a)))),"
          /* modulus_sq     */ "(modulus_sq_{1}(a) + modulus_sq_{1}(b)))",
          numberSystem,
          elementSystem);
      }

      template <typename T, typename... P>
      static void setKernelParameters(cl::Kernel & kernel, cl_uint idx, T & arg, P&... moreArgs);
      template <typename T> requires(!cltypes::HasKernelArgHandler<T>)
        static void setKernelParameters(cl::Kernel & kernel, cl_uint idx, T & arg);
      template <typename T> requires(cltypes::HasKernelArgHandler<T>)
        static void setKernelParameters(cl::Kernel & kernel, cl_uint idx, T & arg);

      CLSolver::CLSolver(GPUContext & ctx) :
        // comma operator to set status text before constructor builds the CL program
        ctx((App::get<wxStatusBar>().PushStatusText("Creating OpenCL solver program..."), ctx)),
        program(ctx.buildProgram(
          "z = add(sqr(z), c)",
          cayleyDicksonConstruction("complex", "real") +
          cayleyDicksonConstruction("quaternion", "complex") +
          cayleyDicksonConstruction("octonian", "quaternion") +
          defineMulticomplexNumberSystem("bicomplex", "complex") +
          defineMulticomplexNumberSystem("tricomplex", "bicomplex"),
          8.0)),
        memory(App::get<Settings>().getRenderVolumeSize(), ctx),
        currentPhaseIteration(0),
        currentDynamicalIteration(0) {
        updateKernels();
        App::get<wxStatusBar>().PopStatusText();
      }

      void CLSolver::updateResolution() {
        ctx.queue.finish(); // make sure we don't free mapped SVM buffers.
        memory.free();
        memory = Memory(App::get<Settings>().getRenderVolumeSize(), ctx);
        currentPhaseIteration = 0;
        currentDynamicalIteration = 0;
        setKernelParameters(phaseKernel, 0, memory.phaseClGlTexture(), memory.buffer);
        setKernelParameters(dynamicalKernel, 0, memory.dynamicalClGlTexture(), memory.buffer);
      }

      void CLSolver::updateKernels() {
        Settings& settings = App::get<Settings>();
        phaseKernel = cl::Kernel(program, settings.getPhaseKernelName());
        dynamicalKernel = cl::Kernel(program, settings.getDynamicalKernelName());
        setKernelParameters(phaseKernel, 0, memory.phaseClGlTexture(), memory.buffer);
        setKernelParameters(dynamicalKernel, 0, memory.dynamicalClGlTexture(), memory.buffer);
        updatePhaseView();
        updateDynamicalView();
        updateParameter();
      }

      void CLSolver::updatePhaseView() {
        setKernelParameters(phaseKernel, 2, App::get<Settings>().phaseView.unwrap());
        currentPhaseIteration = 0;
      }

      void CLSolver::updateDynamicalView() {
        setKernelParameters(dynamicalKernel, 2, App::get<Settings>().dynamicalView.unwrap());
        currentDynamicalIteration = 0;
      }

      void CLSolver::updateParameter() {
        cltypes::Number& parameter = App::get<Settings>().parameter;
        setKernelParameters(phaseKernel, 3, parameter);
        setKernelParameters(dynamicalKernel, 3, parameter);
        currentDynamicalIteration = 0;
      }

      CLSolver::~CLSolver() {}

      CLSolver::Result& CLSolver::execute() {
        Settings& settings = App::get<Settings>();
        cl_uint iterationsPerFrame = settings.getIterationsPerFrame();
        cl_uint maxPhaseIterations = settings.getMaxPhaseIterations();
        cl_uint maxDynamicalIterations = settings.getMaxDynamicalIterations();

        cl_uint maxPhaseIterationThisFrame = std::min(currentPhaseIteration + iterationsPerFrame, maxPhaseIterations);
        cl_uint maxDynamicalIterationThisFrame = std::min(currentDynamicalIteration + iterationsPerFrame, maxDynamicalIterations);

        bool executePhaseKernel = settings.phaseEnabled() && currentPhaseIteration < maxPhaseIterations;
        bool executeDynamicalKernel = settings.dynamicalEnabled() && currentDynamicalIteration < maxDynamicalIterations;
        if (executePhaseKernel || executeDynamicalKernel) {
          try {
            if (executePhaseKernel) {
              memory.result.phase.clear();
            }
            if (executeDynamicalKernel) {
              memory.result.dynamical.clear();
            }
            glFinish();
            ctx.queue.enqueueAcquireGLObjects(&memory.clGlTextures);
            if (executePhaseKernel) {
              memory.buffer.useBuffer(0);
              setKernelParameters(
                phaseKernel,
                4,
                currentPhaseIteration,
                maxPhaseIterationThisFrame);
              ctx.queue.enqueueNDRangeKernel(
                phaseKernel,
                cl::NullRange,
                memory.range);
              currentPhaseIteration = maxPhaseIterationThisFrame;
            }
            if (executeDynamicalKernel) {
              memory.buffer.useBuffer(1);
              setKernelParameters(
                dynamicalKernel,
                4,
                currentDynamicalIteration,
                maxDynamicalIterationThisFrame);
              ctx.queue.enqueueNDRangeKernel(
                dynamicalKernel,
                cl::NullRange,
                memory.range);
              currentDynamicalIteration = maxDynamicalIterationThisFrame;
            }
            ctx.queue.enqueueReleaseGLObjects(&memory.clGlTextures);
            ctx.queue.finish();
          }
          catch (const cl::Error& e) {
            throw CLError("Could not execute kernel", e);
          }
        }
        return memory.result;
      }

      CLSolver::Result::Result(cl::NDRange & range) :
        phase(range[0], range[1], range[2]),
        dynamical(range[0], range[1], range[2]) {
      }

      template <typename T> requires(!cltypes::HasKernelArgHandler<T>)
        void setKernelParameters(cl::Kernel & kernel, cl_uint index, T & arg) {
        try {
          kernel.setArg(index, arg);
        }
        catch (const cl::Error& e) {
          throw CLKernelArgError("", kernel, index, e);
        }
      }

      template<typename T> requires(cltypes::HasKernelArgHandler<T>)
        void setKernelParameters(cl::Kernel & kernel, cl_uint index, T & arg) {
        arg.asKernelArg(kernel, index);
      }

      template <typename T, typename... P>
      void setKernelParameters(cl::Kernel & kernel, cl_uint index, T & arg, P&... moreArgs) {
        setKernelParameters(kernel, index, arg);
        setKernelParameters(kernel, index + 1, moreArgs...);
      }

      CLSolver::Memory::Memory(cl::NDRange range, GPUContext & ctx) :
        range(range),
        result(range),
        clGlTextures{ result.phase(ctx.clCtx), result.dynamical(ctx.clCtx) },
        buffer(ctx, range) {}

      void CLSolver::Memory::free() {
        buffer.free();
        clGlTextures.clear();
        result.phase.free();
        result.dynamical.free();
      }
    }
  }
}