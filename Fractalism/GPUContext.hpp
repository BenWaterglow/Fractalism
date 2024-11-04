#ifndef _GPU_CONTEXT_HPP_
#define _GPU_CONTEXT_HPP_

#include "CLIncludeHelper.hpp"

#include "WxIncludeHelper.hpp"

// we don't use glew in this header, but it must be included before gl.h
#include <GL/glew.h>
#include <wx/glcanvas.h>

struct GPUContext {
  GPUContext(wxGLCanvas& canvas, wxStatusBar& statusBar);
  cl::Program buildProgram(const char* src, const char* numberSystemDefinitions, size_t maxNumberSystemSize, double escapeValue) const;
  wxGLContext glCtx;             ///< OpenGL context for device communication.
  cl::Device device;             ///< OpenCL device used for computation.
  cl::Context clCtx;             ///< OpenCL context for device communication.
  cl::CommandQueue queue;        ///< Queue to manage OpenCL command execution.
  cl_ulong maxMemAllocSize;      ///< Maximum memory allocatable on the device.
};

#endif