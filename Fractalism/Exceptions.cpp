#include "Exceptions.hpp"
#include "CLUtils.hpp"

FractalismError::FractalismError(const char * const &what) : std::exception(what) {}

GLError::GLError(const char * const &what) : FractalismError(what) {}

GLBuildError::GLBuildError(const char * const &what) : GLError(what) {}

GLCompileError::GLCompileError(const char * const &what) : GLBuildError(what) {}

GLLinkError::GLLinkError(const char * const &what) : GLBuildError(what) {}

CLError::CLError(const char * const &what) : FractalismError(what) {}

CLError::CLError(const char * const &message, const cl::Error& e) : CLError("%s. %s encountered error %s (%d)", message, e.what(), clutils::getCLErrorString(e.err()), e.err()) {}

CLBuildError::CLBuildError(const char * const &what) : CLError(what) {}

CLSVMAllocationError::CLSVMAllocationError(const char * const &what) : CLError(what) {}