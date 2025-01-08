#ifndef _FRACTALISM_CL_COMMON_HPP_
#define _FRACTALISM_CL_COMMON_HPP_

#pragma warning(push, 0)
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_TARGET_OPENCL_VERSION 220
#define CL_HPP_TARGET_OPENCL_VERSION CL_TARGET_OPENCL_VERSION
#if defined(__APPLE__)
#include <OpenCL/opencl.hpp>
#else
#include <CL/opencl.hpp>
#include <CL/cl_gl.h>
#endif
#pragma warning(pop)

#endif // end include guard.
