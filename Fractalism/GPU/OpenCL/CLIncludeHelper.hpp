#ifndef _FRACTALISM_CL_INCLUDE_HELPER_HPP_
#define _FRACTALISM_CL_INCLUDE_HELPER_HPP_

#pragma warning(push)
#pragma warning(disable : 4510)
#pragma warning(disable : 4512)
#pragma warning(disable : 4610)
#pragma warning(disable : 26439)
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_TARGET_OPENCL_VERSION 220
#define CL_HPP_TARGET_OPENCL_VERSION CL_TARGET_OPENCL_VERSION
#if defined(__APPLE__)
#include <OpenCL/opencl.hpp>
#else
#include <CL/opencl.hpp>
#endif
#pragma warning(pop)

#endif // !_CL_INCLUDE_HELPER_HPP_
