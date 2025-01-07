#include <format>
#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/GPU/OpenCL/CLUtils.hpp>

namespace fractalism {

  FractalismError::FractalismError(
      const std::string& what,
      const std::source_location where) :
        std::runtime_error(std::format("{}\nAt {} {}:{} ({})",
          what,
          where.file_name(),
          where.line(),
          where.column(),
          where.function_name())) {}

  AssertionError::AssertionError(
      const std::string& what,
      const std::source_location where) :
        FractalismError(what, where) {}

  GLError::GLError(
      const std::string& what,
      const std::source_location where) :
        FractalismError(what, where) {}

  GLError::GLError(
      const std::string& message,
      GLenum errorCode,
      const std::source_location where) :
        GLError(std::format("{}. OpenGL encounterd error {} ({})",
            message,
            reinterpret_cast<const char*>(glewGetErrorString(errorCode)),
            errorCode),
          where) {}

  GLError::GLError(
      GLenum errorCode,
      const std::source_location where) :
        GLError("Unexpected error", errorCode, where) {}

  GLBuildError::GLBuildError(
      const std::string& what,
      const std::source_location where) :
        GLError(what, where) {}

  GLCompileError::GLCompileError(
      GLenum type,
      GLuint shaderId,
      const std::source_location where) :
        GLBuildError(std::format("Could not compile OpenGL {} shader. Compiler log:\n{}",
            gpu::opengl::glutils::getShaderType(type),
            gpu::opengl::glutils::getGlCompilerLog(shaderId)),
          where) {}

  GLLinkError::GLLinkError(
      GLuint programId,
      const std::source_location where) :
        GLBuildError(std::format("Could not link OpenGL program. Linker log:\n{}",
            gpu::opengl::glutils::getLinkerLog(programId)),
          where) {}

  CLError::CLError(
      const std::string& what,
      const std::source_location where) :
        FractalismError(what, where) {}

  CLError::CLError(
      const std::string& message,
      const cl::Error& e,
      const std::source_location where) :
    CLError(std::format("{}. OpenCL function {} encountered error {} ({})",
      message,
      e.what(), // cl::Error.what() just returns the function name.
      gpu::opencl::clutils::getCLErrorString(e.err()),
      e.err()),
      where) {}

  CLBuildError::CLBuildError(
      const std::string& what,
      const std::source_location where) :
        CLError(what, where) {}

  CLSVMAllocationError::CLSVMAllocationError(
      const std::string& what,
      const std::source_location where) :
        CLError(what, where) {}

  CLKernelArgError::CLKernelArgError(
      const std::string& what,
      const std::source_location where) :
        CLError(what, where) {}

  CLKernelArgError::CLKernelArgError(
      const std::string& message,
      cl::Kernel& kernel,
      cl_uint index,
      const cl::Error& e,
      const std::source_location where) :
        CLError(std::format("Could not set {} for kernel '{}' argument #{} ({} {})",
            message,
            kernel.getInfo<CL_KERNEL_FUNCTION_NAME>(),
            index,
            kernel.getArgInfo<CL_KERNEL_ARG_TYPE_NAME>(index),
            kernel.getArgInfo<CL_KERNEL_ARG_NAME>(index)),
          e,
          where) {}
}