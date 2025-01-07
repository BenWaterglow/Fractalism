#ifndef _FRACTALISM_GPU_HPP_
#define _FRACTALISM_GPU_HPP_

#include <Fractalism/UI/UICommon.hpp>

// we don't use glew in this header, but it must be included before gl.h
#include <GL/glew.h>
#include <wx/glcanvas.h>


#include <Fractalism/GPU/GPUContext.hpp>
#include <Fractalism/GPU/OpenCL/ProgramManager.hpp>
#include <Fractalism/GPU/OpenGL/GLRenderer.hpp>
#include <Fractalism/GPU/OpenGL/GLShaderProgram.hpp>

namespace fractalism::gpu {
  struct GPU {
    GPU(wxGLCanvas& canvas);
    void reloadShaders();
    const GPUContext ctx;
    opengl::GLShaderProgram shader2D;
    opengl::GLShaderProgram shader3D;
    opengl::GLRenderer renderer;
    opencl::ProgramManager kernelManager;
  };
}

#endif