#ifndef _EXCEPTIONS_HPP_
#define _EXCEPTIONS_HPP_

#include <string>
#include <exception>

#include <GL/glew.h>

#include "Utils.hpp"
#include "CLIncludeHelper.hpp"

class FractalismError : public std::exception {
public:
  FractalismError(const char * const &what);
  template <typename... T>
  FractalismError(const char * const &format, T...args) : FractalismError(utils::format(format, args...).get()) {}
};

class GLError : public FractalismError {
public:
  GLError(const char * const &what);
  template <typename... T>
  GLError(const char * const &format, T...args) : FractalismError(format, args...) {}
};

class GLBuildError : public GLError {
public:
  GLBuildError(const char * const &what);
  template <typename... T>
  GLBuildError(const char * const &format, T...args) : GLError(format, args...) {}
};

class GLCompileError : public GLBuildError {
public:
  GLCompileError(const char * const &what);
  template <typename... T>
  GLCompileError(const char * const &format, T...args) : GLBuildError(format, args...) {}
};

class GLLinkError : public GLBuildError {
public:
  GLLinkError(const char * const &what);
  template <typename... T>
  GLLinkError(const char * const &format, T...args) : GLBuildError(format, args...) {}
};

class CLError : public FractalismError {
public:
  CLError(const char * const &what);
  CLError(const char * const &message, const cl::Error& e);
  template <typename... T>
  CLError(const char * const &format, T...args) : FractalismError(format, args...) {}
};

class CLBuildError : public CLError {
public:
  CLBuildError(const char * const &what);
  template <typename... T>
  CLBuildError(const char * const &format, T...args) : CLError(format, args...) {}
};

class CLSVMAllocationError : public CLError {
public:
  CLSVMAllocationError(const char * const &what);
  template <typename... T>
  CLSVMAllocationError(const char * const &format, T...args) : CLError(format, args...) {}
};

#endif