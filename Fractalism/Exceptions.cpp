#include <format>
#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/GPU/OpenCL/CLUtils.hpp>

namespace fractalism {

  FractalismError::FractalismError(const std::string& what) : std::runtime_error(what) {}

  AssertionError::AssertionError(const std::string& what) : FractalismError(what) {}

  GLError::GLError(const std::string& what) : FractalismError(what) {}

  GLError::GLError(const std::string& message, GLenum errorCode) :
    GLError(std::format("{}. OpenGL encounterd error {} ({})",
      message,
      reinterpret_cast<const char*>(glewGetErrorString(errorCode)),
      errorCode)) {}

  GLError::GLError(GLenum errorCode) :
    GLError("Unexpected error", errorCode) {}

  GLBuildError::GLBuildError(const std::string& what) : GLError(what) {}

  GLCompileError::GLCompileError(GLenum type, GLuint shaderId) :
    GLBuildError(std::format("Could not compile OpenGL {} shader. Compiler log:\n{}",
      gpu::opengl::glutils::getShaderType(type),
      gpu::opengl::glutils::getGlCompilerLog(shaderId))) {}

  GLLinkError::GLLinkError(GLuint programId) :
    GLBuildError(std::format("Could not link OpenGL program. Linker log:\n{}",
      gpu::opengl::glutils::getLinkerLog(programId))) {}

  CLError::CLError(const std::string& what) : FractalismError(what) {}

  CLError::CLError(const std::string& message, const cl::Error& e) :
    CLError(std::format("{}. OpenCL function {} encountered error {} ({})",
      message,
      e.what(), // cl::Error.what() just returns the function name.
      gpu::opencl::clutils::getCLErrorString(e.err()),
      e.err())) {}

  CLBuildError::CLBuildError(const std::string& what) : CLError(what) {}

  CLSVMAllocationError::CLSVMAllocationError(const std::string& what) : CLError(what) {}

  CLKernelArgError::CLKernelArgError(const std::string& what) : CLError(what) {}

  CLKernelArgError::CLKernelArgError(const std::string& message, cl::Kernel& kernel, cl_uint index, const cl::Error& e) :
    CLError(std::format("Could not set {} for kernel '{}' argument #{} ({} {})",
      message,
      kernel.getInfo<CL_KERNEL_FUNCTION_NAME>(),
      index,
      kernel.getArgInfo<CL_KERNEL_ARG_TYPE_NAME>(index),
      kernel.getArgInfo<CL_KERNEL_ARG_NAME>(index)), e) {}
}