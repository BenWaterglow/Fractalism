#ifndef _FRACTALISM_EXCEPTIONS_HPP_
#define _FRACTALISM_EXCEPTIONS_HPP_

#include <string>
#include <stdexcept>
#include <GL/glew.h>

#include <Fractalism/GPU/OpenCL/CLIncludeHelper.hpp>

namespace fractalism {
  class FractalismError : public std::runtime_error {
  public:
    FractalismError(const std::string& what);
  };

  class AssertionError : public FractalismError {
  public:
    AssertionError(const std::string& what);
  };

  class GLError : public FractalismError {
  public:
    GLError(const std::string& what);
    GLError(const std::string& message, GLenum errorCode);
    GLError(GLenum errorCode);
  };

  class GLBuildError : public GLError {
  public:
    GLBuildError(const std::string& what);
  };

  class GLCompileError : public GLBuildError {
  public:
    GLCompileError(GLenum type, GLuint shaderId);
  };

  class GLLinkError : public GLBuildError {
  public:
    GLLinkError(GLuint programId);
  };

  class CLError : public FractalismError {
  public:
    CLError(const std::string& what);
    CLError(const std::string& what, const cl::Error& e);
  };

  class CLBuildError : public CLError {
  public:
    CLBuildError(const std::string& what);
  };

  class CLKernelArgError : public CLError {
  public:
    CLKernelArgError(const std::string& what);
    CLKernelArgError(const std::string& message, cl::Kernel& kernel, cl_uint index, const cl::Error& e);
  };

  class CLSVMAllocationError : public CLError {
  public:
    CLSVMAllocationError(const std::string& what);
  };
}

#endif