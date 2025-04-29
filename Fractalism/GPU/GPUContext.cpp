#include <Fractalism/GPU/GPUContext.hpp>

#include <format>

#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/App.hpp>

namespace fractalism::gpu {
  static inline cl::Context createContext(cl::Platform& platform, cl::Device& device, const wxGLContext& glCtx, wxGLCanvas& canvas) {
    cl_context_properties ctxProps[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
        #if defined(__WXMSW__)
          CL_GL_CONTEXT_KHR, (cl_context_properties)glCtx.GetGLRC(),
          CL_WGL_HDC_KHR, (cl_context_properties)canvas.GetHDC(),
        #elif defined(__WXMOTIF__) || defined(__WXX11__)
          CL_GLX_DISPLAY_KHR, (cl_context_properties)canvas.GetXWindow(),
        /* Copied from wx/glcanvas.h, add the GL context and DC for these ports
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

  GPUContext::GPUContext(wxGLCanvas& canvas) :
        glCtx(App::doWithStatusMessage("Initializing OpenGL context...", [](wxGLCanvas& canvas) {return wxGLContext(&canvas);}, canvas)) {
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

    App::doWithStatusMessage("Initializing OpenCL context...", [](GPUContext& ctx, wxGLCanvas& canvas) {
      try {
        // TODO: check platform.getInfo<CL_PLATFORM_EXTENSIONS>()
        cl::Platform platform = cl::Platform::get();

        cl_device_id deviceId = 0;
        // Try to get the current GL context device
        clGetGLContextInfoKHR_fn clGetGLContextInfo = reinterpret_cast<clGetGLContextInfoKHR_fn>(
            clGetExtensionFunctionAddressForPlatform(platform(), "clGetGLContextInfoKHR"));
        if (clGetGLContextInfo) {
          cl_context_properties props[] = {
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
            0
          };
          clGetGLContextInfo(props, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_uint), &deviceId, nullptr);
          if (deviceId) {
            ctx.device = deviceId;
          }
        }
        // If we couldn't get the proper device, use a bad and terrible fallback.
        if (ctx.device()) {
          ctx.clCtx = createContext(platform, ctx.device, ctx.glCtx, canvas);
        } else {
          std::vector<cl::Device> devices;
          platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
          for (cl::Device& device : devices) {
            try {
              ctx.clCtx = createContext(platform, device, ctx.glCtx, canvas);
              ctx.device = device;
              break;
            } catch (const cl::Error&) {
              continue;
            }
          }
          if (!ctx.device()) {
            throw CLError("Could not find CL/GL compatible device");
          }
        }

        // TODO: verify SVM support
        ctx.maxMemAllocSize = ctx.device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();

        ctx.queue = cl::CommandQueue(ctx.clCtx, ctx.device);
      }
      catch (const cl::Error& e) {
        throw CLError("Could not create OpenCL context", e);
      }
    }, *this, canvas);
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

  cl::Program GPUContext::buildProgram(
      const std::string&& function,
      const std::string&& numberSystemDefinitions,
      double escapeValue) const {
    return App::doWithStatusMessage("Creating OpenCL solver program...", [](
        const GPUContext& ctx,
        const std::string& function,
        const std::string& numberSystemDefinitions,
        double escapeValue) -> cl::Program {
      cl::Program program;
      try {
        program = cl::Program(ctx.clCtx, std::format(
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
          ctx.maxMemAllocSize,
          escapeValue,
          numberSystemDefinitions,
          function));
      } catch (const cl::Error& e) {
        if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
          writeBuildLog(ctx.device, program);
          throw CLBuildError("Could not build OpenCL program. See cl_build.log for more information.");
        } else {
          throw CLError("Could not build OpenCL program", e);
        }
      }
      program.build("-I KernelHeaders -cl-kernel-arg-info");
      writeBuildLog(ctx.device, program);
      return program;
    }, *this, function, numberSystemDefinitions, escapeValue);
  }
}