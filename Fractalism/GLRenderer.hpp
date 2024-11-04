#ifndef _GL_RENDERER_HPP_
#define _GL_RENDERER_HPP_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "CLSolver.hpp"
#include "GLShaderProgram.hpp"
#include "CLTypes.hpp"
#include "Options.hpp"
#include "SharedState.hpp"

class GLRenderer {
public:
  GLRenderer(options::Dimensions dimension, SharedState::RenderState& renderState);
  ~GLRenderer();
  void render(int width, int height, GLuint texture);
  virtual void changeView(double deltaX, double deltaY) = 0;
  virtual void changeParameter(double x, double y) = 0;
  const options::Dimensions dimension;
protected:
  virtual void doRender(int width, int height) = 0;
  virtual void setupVBOs(GLuint& verticesVBO, GLuint& indicesVBO) = 0;
  SharedState::RenderState& renderState;
private:
  void setup();
  inline void nothing() {}
  decltype(&GLRenderer::setup) maybeSetup;
  GLShaderProgram& shader;
  GLuint VAO;
  GLuint VBOs[2];
};

class GLRenderer2D : public GLRenderer {
public:
  GLRenderer2D(SharedState::RenderState& renderState);
  ~GLRenderer2D();
  virtual void changeView(double deltaX, double deltaY) override;
  virtual void changeParameter(double x, double y) override;
protected:
  virtual void doRender(int width, int height) override;
  virtual void setupVBOs(GLuint& verticesVBO, GLuint& indicesVBO) override;
};

class GLRenderer3D : public GLRenderer {
public:
  GLRenderer3D(SharedState::RenderState& renderState);
  ~GLRenderer3D();
  virtual void changeView(double deltaX, double deltaY) override;
  virtual void changeParameter(double x, double y) override;
protected:
  virtual void doRender(int width, int height) override;
  virtual void setupVBOs(GLuint& verticesVBO, GLuint& indicesVBO) override;
private:
  glm::dvec3 eye;
};

#endif