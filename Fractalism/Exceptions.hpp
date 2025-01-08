#ifndef _FRACTALISM_EXCEPTIONS_HPP_
#define _FRACTALISM_EXCEPTIONS_HPP_

#include <Fractalism/GPU/OpenCL/CLCommon.hpp>
#include <GL/glew.h>
#include <source_location>
#include <stdexcept>
#include <string>

namespace fractalism {

/**
 * @class FractalismError
 * @brief Base class for all Fractalism exceptions.
 */
class FractalismError : public std::runtime_error {
public:
  /**
   * @brief Constructs a FractalismError.
   * @param what The error message.
   * @param where The source location where the error occurred.
   */
  FractalismError(
      const std::string& what,
      const std::source_location where = std::source_location::current());
};

/**
 * @class AssertionError
 * @brief Exception thrown when an assertion fails.
 */
class AssertionError : public FractalismError {
public:
  /**
   * @brief Constructs an AssertionError.
   * @param what The error message.
   * @param where The source location where the error occurred.
   */
  AssertionError(
      const std::string& what,
      const std::source_location where = std::source_location::current());
};

/**
 * @class GLError
 * @brief Exception thrown for OpenGL errors.
 */
class GLError : public FractalismError {
public:
  /**
   * @brief Constructs a GLError.
   * @param what The error message.
   * @param where The source location where the error occurred.
   */
  GLError(
      const std::string& what,
      const std::source_location where = std::source_location::current());

  /**
   * @brief Constructs a GLError with an error code.
   * @param message The error message.
   * @param errorCode The OpenGL error code.
   * @param where The source location where the error occurred.
   */
  GLError(
      const std::string& message,
      GLenum errorCode,
      const std::source_location where = std::source_location::current());

  /**
   * @brief Constructs a GLError with an error code.
   * @param errorCode The OpenGL error code.
   * @param where The source location where the error occurred.
   */
  GLError(
      GLenum errorCode,
      const std::source_location where = std::source_location::current());
};

/**
 * @class GLBuildError
 * @brief Exception thrown for OpenGL build errors.
 */
class GLBuildError : public GLError {
public:
  /**
   * @brief Constructs a GLBuildError.
   * @param what The error message.
   * @param where The source location where the error occurred.
   */
  GLBuildError(
      const std::string& what,
      const std::source_location where = std::source_location::current());
};

/**
 * @class GLCompileError
 * @brief Exception thrown for OpenGL shader compilation errors.
 */
class GLCompileError : public GLBuildError {
public:
  /**
   * @brief Constructs a GLCompileError.
   * @param type The type of the shader.
   * @param shaderId The ID of the shader.
   * @param where The source location where the error occurred.
   */
  GLCompileError(
      GLenum type,
      GLuint shaderId,
      const std::source_location where = std::source_location::current());
};

/**
 * @class GLLinkError
 * @brief Exception thrown for OpenGL program linking errors.
 */
class GLLinkError : public GLBuildError {
public:
  /**
   * @brief Constructs a GLLinkError.
   * @param programId The ID of the program.
   * @param where The source location where the error occurred.
   */
  GLLinkError(
      GLuint programId,
      const std::source_location where = std::source_location::current());
};

/**
 * @class CLError
 * @brief Exception thrown for OpenCL errors.
 */
class CLError : public FractalismError {
public:
  /**
   * @brief Constructs a CLError.
   * @param what The error message.
   * @param where The source location where the error occurred.
   */
  CLError(
      const std::string& what,
      const std::source_location where = std::source_location::current());

  /**
   * @brief Constructs a CLError with an OpenCL error.
   * @param what The error message.
   * @param e The OpenCL error.
   * @param where The source location where the error occurred.
   */
  CLError(
      const std::string& what,
      const cl::Error& e,
      const std::source_location where = std::source_location::current());
};

/**
 * @class CLBuildError
 * @brief Exception thrown for OpenCL build errors.
 */
class CLBuildError : public CLError {
public:
  /**
   * @brief Constructs a CLBuildError.
   * @param what The error message.
   * @param where The source location where the error occurred.
   */
  CLBuildError(
      const std::string& what,
      const std::source_location where = std::source_location::current());
};

/**
 * @class CLKernelArgError
 * @brief Exception thrown for OpenCL kernel argument errors.
 */
class CLKernelArgError : public CLError {
public:
  /**
   * @brief Constructs a CLKernelArgError.
   * @param what The error message.
   * @param where The source location where the error occurred.
   */
  CLKernelArgError(
      const std::string& what,
      const std::source_location where = std::source_location::current());

  /**
   * @brief Constructs a CLKernelArgError with a kernel and index.
   * @param message The error message.
   * @param kernel The OpenCL kernel.
   * @param index The argument index.
   * @param e The OpenCL error.
   * @param where The source location where the error occurred.
   */
  CLKernelArgError(
      const std::string& message,
      cl::Kernel& kernel,
      cl_uint index,
      const cl::Error& e,
      const std::source_location where = std::source_location::current());
};

/**
 * @class CLSVMAllocationError
 * @brief Exception thrown for OpenCL SVM allocation errors.
 */
class CLSVMAllocationError : public CLError {
public:
  /**
   * @brief Constructs a CLSVMAllocationError.
   * @param what The error message.
   * @param where The source location where the error occurred.
   */
  CLSVMAllocationError(
      const std::string& what,
      const std::source_location where = std::source_location::current());
};
} // namespace fractalism

#endif
