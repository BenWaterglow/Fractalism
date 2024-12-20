#ifndef _FRACTALISM_GPU_CONTEXT_HPP_
#define _FRACTALISM_GPU_CONTEXT_HPP_

#include <string>

#include <Fractalism/GPU/OpenCL/CLIncludeHelper.hpp>

#include <Fractalism/UI/WxIncludeHelper.hpp>

// we don't use glew in this header, but it must be included before gl.h
#include <GL/glew.h>
#include <wx/glcanvas.h>

namespace fractalism {
  namespace gpu {
    struct GPUContext {
      GPUContext(wxGLCanvas& canvas);
      cl::Program buildProgram(const std::string&& function, const std::string&& numberSystemDefinitions, double escapeValue) const;
      wxGLContext glCtx;             ///< OpenGL context for device communication.
      cl::Device device;             ///< OpenCL device used for computation.
      cl::Context clCtx;             ///< OpenCL context for device communication.
      cl::CommandQueue queue;        ///< Queue to manage OpenCL command execution.
      cl_ulong maxMemAllocSize;      ///< Maximum memory allocatable on the device.
    };
  }
}
#endif