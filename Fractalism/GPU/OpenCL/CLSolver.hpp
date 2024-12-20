#ifndef _FRACTALISM_CL_SOLVER_HPP_
#define _FRACTALISM_CL_SOLVER_HPP_

#include <Fractalism/GPU/OpenCL/CLTypes.hpp>
#include <Fractalism/GPU/OpenCL/SVMPtr.hpp>
#include <Fractalism/GPU/OpenGL/GLTexture3D.hpp>
#include <Fractalism/GPU/GPUContext.hpp>

namespace fractalism {
  namespace gpu {
    namespace opencl {

      /**
       * @class CLSolver
       * @brief Manages OpenCL contexts, devices, and kernel execution for fractal calculations.
       *
       * Initializes the OpenCL environment and provides an interface to create and manage
       * OpenCL programs for rendering fractals. Supports OpenCL-OpenGL interop.
       */
      class CLSolver {
      public:

        /**
         * @struct Result
         * @brief Holds the textures containing the solver results.
         */
        struct Result {
          /**
           * @brief Constructs Output textures with given dimensions.
           * @param range Dimensions of the textures to create.
           */
          Result(cl::NDRange& range);
          opengl::GLTexture3D phase;      ///< Texture storing the phase space fractal
          opengl::GLTexture3D dynamical;  ///< Texture storing the dynamical space fractal.
        };

        /**
         * @brief Constructs a CLSolver instance and initializes OpenCL contexts and devices.
         * @param ctx The OpenGL and OpenCL context objects.
         */
        CLSolver(GPUContext& ctx);

        /**
         * @brief Destructor for the CLSolver, releasing allocated resources.
         */
        ~CLSolver();

        void updateResolution();
        void updateKernels();
        void updatePhaseView();
        void updateDynamicalView();
        void updateParameter();
        Result& execute();

      private:
        struct Memory {
          Memory(cl::NDRange range, GPUContext& ctx);
          inline cl::Memory& phaseClGlTexture() { return clGlTextures[0]; }
          inline cl::Memory& dynamicalClGlTexture() { return clGlTextures[1]; }
          cl::NDRange range;
          Result result;
          std::vector<cl::Memory> clGlTextures;
          BackBufferedSvmArrayPtr<cltypes::WorkStore> buffer;
          void free();
        };

        Memory memory;                      ///< Contains all managed memory objects.
        GPUContext& ctx;                    ///< OpenGL and OpenCL interface objects.
        cl::Program program;                ///< OpenCL program. Contains all kernels.
        cl_uint currentPhaseIteration;      ///< The iteration that the phase kernel has calculated so far.
        cl_uint currentDynamicalIteration;  ///< The iteration that the dynamical kernel has calculated so far.
        cl::Kernel phaseKernel;             ///< The kernel for calculating in phase space.
        cl::Kernel dynamicalKernel;         ///< The kernel for calculating in dynamical space.
      };
    }
  }
}

#endif