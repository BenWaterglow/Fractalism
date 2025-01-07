#ifndef _FRACTALISM_GPU_CONTEXT_HPP_
#define _FRACTALISM_GPU_CONTEXT_HPP_

#include <string>

#include <Fractalism/GPU/OpenCL/CLCommon.hpp>

#include <Fractalism/UI/UICommon.hpp>

// we don't use glew in this header, but it must be included before gl.h
#include <GL/glew.h>
#include <wx/glcanvas.h>

namespace fractalism::gpu {
  struct GPUContext {
    GPUContext(wxGLCanvas& canvas);
    cl::Program buildProgram(const std::string&& function, const std::string&& numberSystemDefinitions, double escapeValue) const;
    inline void setCurrent(wxGLCanvas& canvas) const {
      canvas.SetCurrent(glCtx);
    }
    inline operator const cl::Context& () const { return clCtx; }
    inline operator const cl_context& () const { return clCtx(); }
    cl::Device device;                ///< OpenCL device used for computation.
    cl::Context clCtx;                ///< OpenCL context for device communication.
    cl::CommandQueue queue;           ///< Queue to manage OpenCL command execution.
    cl_ulong maxMemAllocSize = 0;     ///< Maximum memory allocatable on the device.
  private:
    wxGLContext glCtx;          ///< OpenGL context for device communication.    
  };
}
#endif