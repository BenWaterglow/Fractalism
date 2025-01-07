#include <Fractalism/GPU/GPU.hpp>

#include <Fractalism/App.hpp>

namespace fractalism::gpu {
  GPU::GPU(wxGLCanvas& canvas) :
        ctx(canvas),
        shader2D("Shaders/2d.vert", "Shaders/2d.frag"),
        shader3D("Shaders/3d.vert", "Shaders/3d.frag"),
        renderer(),
        kernelManager(ctx) {
    reloadShaders();
  }

  void GPU::reloadShaders() {
    App::doWithStatusMessage("Loading shaders...", [](opengl::GLShaderProgram& shader2D, opengl::GLShaderProgram& shader3D) {
      shader2D.load();
      shader3D.load();
    }, shader2D, shader3D);
  }
}