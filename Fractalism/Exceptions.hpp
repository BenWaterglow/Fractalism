#ifndef _FRACTALISM_EXCEPTIONS_HPP_
#define _FRACTALISM_EXCEPTIONS_HPP_

#include <string>
#include <stdexcept>
#include <source_location>
#include <GL/glew.h>

#include <Fractalism/GPU/OpenCL/CLCommon.hpp>

namespace fractalism {
  class FractalismError : public std::runtime_error {
  public:
    FractalismError(
        const std::string& what,
        const std::source_location where = std::source_location::current());
  };

  class AssertionError : public FractalismError {
  public:
    AssertionError(
        const std::string& what,
        const std::source_location where = std::source_location::current());
  };

  class GLError : public FractalismError {
  public:
    GLError(
        const std::string& what,
        const std::source_location where = std::source_location::current());
    GLError(
        const std::string& message,
        GLenum errorCode,
        const std::source_location where = std::source_location::current());
    GLError(
        GLenum errorCode,
        const std::source_location where = std::source_location::current());
  };

  class GLBuildError : public GLError {
  public:
    GLBuildError(
        const std::string& what,
        const std::source_location where = std::source_location::current());
  };

  class GLCompileError : public GLBuildError {
  public:
    GLCompileError(
        GLenum type,
        GLuint shaderId,
        const std::source_location where = std::source_location::current());
  };

  class GLLinkError : public GLBuildError {
  public:
    GLLinkError(
        GLuint programId,
        const std::source_location where = std::source_location::current());
  };

  class CLError : public FractalismError {
  public:
    CLError(
        const std::string& what,
        const std::source_location where = std::source_location::current());
    CLError(
        const std::string& what,
        const cl::Error& e,
        const std::source_location where = std::source_location::current());
  };

  class CLBuildError : public CLError {
  public:
    CLBuildError(
        const std::string& what,
        const std::source_location where = std::source_location::current());
  };

  class CLKernelArgError : public CLError {
  public:
    CLKernelArgError(
        const std::string& what,
        const std::source_location where = std::source_location::current());
    CLKernelArgError(
        const std::string& message,
        cl::Kernel& kernel,
        cl_uint index,
        const cl::Error& e,
        const std::source_location where = std::source_location::current());
  };

  class CLSVMAllocationError : public CLError {
  public:
    CLSVMAllocationError(
        const std::string& what,
        const std::source_location where = std::source_location::current());
  };
}

#endif