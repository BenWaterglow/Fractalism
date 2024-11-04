#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4201)
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

#include "GLRenderer.hpp"
#include "GLUtils.hpp"
#include "Exceptions.hpp"
#include "Utils.hpp"

template<GLint id, typename T>
struct Uniform {};

#define UNIFORM_TYPE(T, fn, size) \
template<GLint id> \
struct Uniform<id, T> { \
  template<typename V> \
  static constexpr Uniform<id + size, V> next = {}; \
  inline auto operator=(const T&& value) const { \
    fn; \
    return *this;\
  } \
  inline auto operator=(const T& value) const { \
    fn; \
    return *this;\
  } \
}

UNIFORM_TYPE(int, glUniform1i(id, value), 1);
UNIFORM_TYPE(float, glUniform1f(id, value), 1);
UNIFORM_TYPE(glm::vec2, glUniform2f(id, value.x, value.y), 1);
UNIFORM_TYPE(glm::vec3, glUniform3f(id, value.x, value.y, value.z), 1);
UNIFORM_TYPE(glm::vec4, glUniform4f(id, value.x, value.y, value.z, value.w), 1);
UNIFORM_TYPE(glm::mat3, glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(value)), 1);
UNIFORM_TYPE(glm::mat4, glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value)), 1);

#undef UNIFORM_TYPE

struct Uniforms {
  static constexpr auto texture = Uniform<0, int>{};
  static constexpr auto mvp = texture.next<glm::mat4>;
  struct material {
    static constexpr auto specular = mvp.next<glm::vec3>;
    static constexpr auto shininess = specular.next<float>;
  };
  struct light {
    static constexpr auto position = material::shininess.next<glm::vec3>;
    static constexpr auto ambient = position.next<glm::vec3>;
    static constexpr auto diffuse = ambient.next<glm::vec3>;
    static constexpr auto specular = diffuse.next<glm::vec3>;
  };
  static constexpr auto normalMatrix = light::specular.next<glm::mat3>;
  static constexpr auto eyePosition = normalMatrix.next<glm::vec3>;
};

GLRenderer::GLRenderer(
      options::Dimensions dimension,
      SharedState::RenderState& renderState) :
    maybeSetup(&GLRenderer::setup),
    dimension(dimension),
    renderState(renderState),
    shader(renderState.sharedState.getShader(dimension)),
    VAO(0),
    VBOs{} {}

GLRenderer::~GLRenderer() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(2, VBOs);
}

void GLRenderer::render(int width, int height, GLuint texture) {
  (this->*maybeSetup)();
  glUseProgram(shader);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width, height);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, texture);
  glBindVertexArray(VAO);
  doRender(width, height);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_3D, 0);
  glutils::checkGLError();
}

void GLRenderer::setup() {
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(2, VBOs);
  setupVBOs(VBOs[0], VBOs[1]);
  glVertexAttribPointer(0, utils::toUnderlyingType(dimension), GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glutils::checkGLError();
  maybeSetup = &GLRenderer::nothing;
}

GLRenderer2D::GLRenderer2D(SharedState::RenderState& renderState) :
    GLRenderer(options::Dimensions::two, renderState) {}

GLRenderer2D::~GLRenderer2D() {}

void GLRenderer2D::doRender(int width, int height) {
  UNUSED(width, height);
  Uniforms::texture = 0;
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);
}

void GLRenderer2D::setupVBOs(GLuint& verticesVBO, GLuint& indicesVBO) {
  // Single full-screen quad
  constexpr GLfloat vertices2D[] = {
    -1.0f, -1.0f,
    3.0f, -1.0f,
    -1.0f, 3.0f
  };

  constexpr GLushort indices2D[] = {
    0, 1, 2
  };

  glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2D), vertices2D, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2D), indices2D, GL_STATIC_DRAW);
}

void GLRenderer2D::changeView(double deltaX, double deltaY) {
  cltypes::Viewspace view = renderState.view;
  view.center.raw[abs(view.mapping.x) - 1] += (copysign(1.0, view.mapping.x) * deltaX * view.zoom);
  view.center.raw[abs(view.mapping.y) - 1] += (copysign(1.0, view.mapping.y) * deltaY * view.zoom);
  renderState.view = view;
}

void GLRenderer2D::changeParameter(double x, double y) {
  cltypes::Viewspace view = renderState.view;
  renderState.sharedState.parameter = view.center;
  renderState.sharedState.parameter.value.raw[abs(view.mapping.x) - 1] = x;
  renderState.sharedState.parameter.value.raw[abs(view.mapping.y) - 1] = y;
}

GLRenderer3D::GLRenderer3D(SharedState::RenderState& renderState) :
    GLRenderer(options::Dimensions::three, renderState),
    eye({ 3.0f, 0.2f, 1.2f }) {}

GLRenderer3D::~GLRenderer3D() {}

static constexpr const float zNear = 0.1f;
static constexpr const float zFar = 100.0f;
static constexpr const float fov = 45.0f;
static const float focalLength = -1.0f / glm::tan(glm::pi<float>() / 180.0f * fov / 2.0f);

void GLRenderer3D::doRender(int width, int height) {
  float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
  glm::mat4 view = glm::lookAt(eye, glm::dvec3{ 0.0, 0.0, 0.0 }, glm::dvec3{0.0, 1.0, 0.0});

  glm::mat4 inverseView = glm::inverse(view);

  glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
  
  Uniforms::texture = 0;
  Uniforms::mvp = projection * view;
  Uniforms::material::specular = glm::vec3(1.0f, 1.0f, 1.0f);
  Uniforms::material::shininess = 10.0f;

  Uniforms::light::position = glm::vec3(inverseView * glm::vec4(glm::vec3(1.2f, 1.0f, 2.0f), 0.0f));
  Uniforms::light::ambient = glm::vec3(0.5f, 0.5f, 0.5f);
  Uniforms::light::diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
  Uniforms::light::specular = glm::vec3(1.0f, 1.0f, 1.0f);
  
  Uniforms::normalMatrix = glm::transpose(glm::mat3(inverseView));

  Uniforms::eyePosition = eye;
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
}

void GLRenderer3D::setupVBOs(GLuint& verticesVBO, GLuint& indicesVBO) {
  constexpr GLfloat vertices3D[] = {
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f
  };

  constexpr GLushort indices3D[] = {
    // front
    0, 1, 2,
    0, 2, 3,
    // right
    1, 5, 6,
    1, 6, 2,
    // back
    5, 4, 7,
    5, 7, 6,
    // left
    4, 0, 3,
    4, 3, 7,
    // top
    2, 6, 7,
    2, 7, 3,
    // bottom
    4, 5, 1,
    4, 1, 0
  };

  glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3D), vertices3D, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices3D), indices3D, GL_STATIC_DRAW);
}

void GLRenderer3D::changeView(double deltaX, double deltaY) {
  eye.x += deltaX;
  eye.z += deltaY;
}

void GLRenderer3D::changeParameter(double x, double y) {
  UNUSED(x,y);  
}