#include <Fractalism/GPU/GPUContext.hpp>

#include <format>

#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/App.hpp>

namespace fractalism {
  namespace gpu {
    static inline cl::Context createContext(cl::Platform & platform, cl::Device & device, wxGLContext & glCtx, wxGLCanvas & canvas) {
      cl_context_properties ctxProps[] = {
          CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
          #if defined(__WXMSW__)
            CL_GL_CONTEXT_KHR, (cl_context_properties)glCtx.GetGLRC(),
            CL_WGL_HDC_KHR, (cl_context_properties)canvas.GetHDC(),
            /* Copied from wx/glcanvas.h, add the GL context and DC for these ports
            #elif defined(__WXMOTIF__) || defined(__WXX11__)
              #include <wx/x11/glcanvas.h>
            #elif defined(__WXGTK20__)
              #include <wx/gtk/glcanvas.h>
            #elif defined(__WXGTK__)
              #include <wx/gtk1/glcanvas.h>
            #elif defined(__WXMAC__)
              #include <wx/osx/glcanvas.h>
            #elif defined(__WXQT__)
              #include <wx/qt/glcanvas.h>
            */
            #else
              #error "wxGLCanvas not supported in this wxWidgets port"
            #endif
            0 };
      return cl::Context(device, ctxProps);
    }

    GPUContext::GPUContext(wxGLCanvas & canvas) :
      glCtx((App::get<wxStatusBar>().PushStatusText("Initializing OpenGL context..."), &canvas)) {
      if (!glCtx.IsOK()) {
        throw GLError("Could not create OpenGL context.");
      }
      canvas.SetCurrent(glCtx);

      glewExperimental = true;

      GLenum err = glewInit();

      if (err != GL_NO_ERROR) {
        throw GLError("Could not initialize GLEW", err);
      }

      glEnable(GL_DEPTH_TEST);
      glEnable(GL_ALPHA_TEST);
      glDisable(GL_CULL_FACE);
      glClearColor(0.0, 0.0, 0.0, 0.0);
      opengl::glutils::checkGLError();
      App::get<wxStatusBar>().PopStatusText();

      App::get<wxStatusBar>().PushStatusText("Initializing OpenCL context...");
      try {
        // TODO: check platform.getInfo<CL_PLATFORM_EXTENSIONS>()
        cl::Platform platform = cl::Platform::get();

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        device = devices[0]; // TODO: Get proper device

        clCtx = createContext(platform, device, glCtx, canvas);
        // TODO: verify SVM support
        maxMemAllocSize = device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();

        queue = cl::CommandQueue(clCtx, device);
      }
      catch (const cl::Error& e) {
        throw CLError("Could not create OpenCL context", e);
      }
      App::get<wxStatusBar>().PopStatusText();
    }

    static inline void writeBuildLog(const cl::Device & device, const cl::Program & program) {
      cl::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
      if (log.empty() || log.size() == 1) {
        static constexpr const char successMessage[] = "Build successful, no errors or warnings.";
        utils::writeToFile("cl_build.log", sizeof(successMessage) - 1, successMessage);
      }
      else {
        utils::writeToFile("cl_build.log", log.size() - 1, log.c_str());
      }
    }

    cl::Program GPUContext::buildProgram(const std::string && function, const std::string && numberSystemDefinitions, double escapeValue) const {
      cl::Program program;
      try {
        program = cl::Program(clCtx, std::format(
          #if defined(USE_DOUBLE_MATH)
          "#pragma OPENCL EXTENSION cl_khr_fp64 : enable"
          #endif
          R"SRC(
          #define MAX_NUMBER_SYSTEM_SIZE {}
          #define CL_DEVICE_MAX_MEM_ALLOC_SIZE {}
          #define ESCAPE_VALUE {}
          #define NUMBER_SYSTEMS {}
          #define KERNEL_FUNCTION(add, sub, conj, mul, sqr, scale, modulus_sq) {}

          #include "kernels.h")SRC",
          MAX_NUMBER_SYSTEM_SIZE,
          maxMemAllocSize,
          escapeValue,
          numberSystemDefinitions,
          function));
        program.build("-I KernelHeaders -cl-kernel-arg-info");
        writeBuildLog(device, program);
      }
      catch (const cl::Error& e) {
        if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
          writeBuildLog(device, program);
          throw CLBuildError("Could not build OpenCL program. See cl_build.log for more information.");
        }
        else {
          throw CLError("Could not build OpenCL program", e);
        }
      }
      return program;
    }
  }
}