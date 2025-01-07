#include <cassert>
#include <glm/glm.hpp>
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4201)
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

#include <Fractalism/GPU/OpenGL/GLRenderer.hpp>
#include <Fractalism/GPU/OpenGL/GLUniform.hpp>
#include <Fractalism/GPU/OpenGL/GLUtils.hpp>
#include <Fractalism/Exceptions.hpp>
#include <Fractalism/Utils.hpp>
#include <Fractalism/App.hpp>

namespace fractalism::gpu::opengl {
  struct Uniforms {
    static constexpr auto texture = GLUniform<0, int>{};
    static constexpr auto mvp = texture.next<glm::mat4>;
    struct material {
      static constexpr auto specular = mvp.next<glm::vec3>;
      static constexpr auto shininess = specular.next<float>;
    };
    struct light {
      static constexpr auto direction = material::shininess.next<glm::vec3>;
      static constexpr auto ambient = direction.next<glm::vec3>;
      static constexpr auto diffuse = ambient.next<glm::vec3>;
      static constexpr auto specular = diffuse.next<glm::vec3>;
    };
    static constexpr auto normalMatrix = light::specular.next<glm::mat3>;
    static constexpr auto eyePosition = normalMatrix.next<glm::vec3>;
  };

  static constexpr const float zNear = 0.1f;
  static constexpr const float zFar = 100.0f;
  static constexpr const float fov = 45.0f;

  // Single full-screen triangle (slightly faster than quad)
  static constexpr GLfloat vertices2D[] = {
    -1.0f, -1.0f,
    3.0f, -1.0f,
    -1.0f, 3.0f
  };

  static constexpr GLushort indices2D[] = {
    0, 1, 2
  };

  // AABB of side length 1.0, centered at origin.
  static constexpr GLfloat vertices3D[] = {
  -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
  -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f
  };

  static constexpr GLushort indices3D[] = {
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

  template<typename T, typename V>
  concept ArrayOf = std::is_bounded_array_v<T> && std::is_same_v<std::remove_extent_t<T>, V>;

  template<typename T>
  concept GLfloatArray = ArrayOf<T, GLfloat>;

  template<typename T>
  concept GLushortArray = ArrayOf<T, GLushort>;

  template<typename T>
  concept GLuintArray = ArrayOf<T, GLuint>;

  template<options::Dimensions renderDimensions>
  static inline void setupVBOs(
    const GLuintArray auto & VAOs,
    const GLuintArray auto & VBOs,
    const GLfloatArray auto & vertices,
    const GLushortArray auto & indices) {
    constexpr GLuint dimensionIndex = utils::toUnderlyingType(renderDimensions),
      vaoIndex = dimensionIndex,
      vertexVBOIndex = vaoIndex * 2,
      indexVBOIndex = vertexVBOIndex + 1,
      basisElementCount = dimensionIndex + 2;

    glBindVertexArray(VAOs[vaoIndex]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[vertexVBOIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glutils::checkGLError();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[indexVBOIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
    glutils::checkGLError();

    glVertexAttribPointer(0, basisElementCount, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glutils::checkGLError();
  }

  inline static void setUniforms2D() {
    Uniforms::texture = 0;
    glutils::checkGLError();
  }

  inline static void setUniforms3D(ArcballCamera& camera, real aspectRatio) {

    glm::mat4 view = camera.createViewMatrix();
    glm::mat4 projection = camera.createProjectionMatrix(aspectRatio);

    glm::mat4 inverseView = glm::inverse(view);

    Uniforms::texture = 0;
    Uniforms::mvp = projection * view;
    Uniforms::material::specular = glm::vec3(1.0f, 1.0f, 1.0f);
    Uniforms::material::shininess = 10.0f;

    Uniforms::light::direction = inverseView * glm::vec4(1.2f, 1.0f, 2.0f, 0.0f);
    Uniforms::light::ambient = glm::vec3(0.75f, 0.75f, 0.75f);
    Uniforms::light::diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    Uniforms::light::specular = glm::vec3(1.0f, 1.0f, 1.0f);

    Uniforms::normalMatrix = glm::transpose(glm::mat3(inverseView));

    Uniforms::eyePosition = camera.getPosition();
    glutils::checkGLError();
  }

  GLRenderer::GLRenderer() :
    VAOs{},
    VBOs{} {
    glGenVertexArrays(2, VAOs);
    glGenBuffers(4, VBOs);
    glutils::checkGLError();

    setupVBOs<options::Dimensions::two>(VAOs, VBOs, vertices2D, indices2D);
    setupVBOs<options::Dimensions::three>(VAOs, VBOs, vertices3D, indices3D);

    glutils::checkGLError();
  }

  GLRenderer::~GLRenderer() {
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(4, VBOs);
  }

  void GLRenderer::render(ViewWindowSettings& settings, wxGLCanvas& canvas, GLuint texture) const {
    wxSize size = canvas.GetSize();
    int width = size.GetWidth();
    int height = size.GetHeight();
    App::setGLContext(canvas);
    glutils::checkGLError();
    glUseProgram(App::get<GLShaderProgram>());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texture);

    options::Dimensions renderDimensions = App::get<Settings>().renderDimensions;
    glBindVertexArray(VAOs[utils::toUnderlyingType(renderDimensions)]);
    glutils::checkGLError();
    switch (renderDimensions) {
    case options::Dimensions::two: {
      setUniforms2D();
      glDrawElements(GL_TRIANGLES, sizeof(indices2D) / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);
      break;
    }
    case options::Dimensions::three: {
      setUniforms3D(settings.camera, static_cast<real>(width) / static_cast<real>(height));
      glDrawElements(GL_TRIANGLES, sizeof(indices3D) / sizeof(GLushort), GL_UNSIGNED_SHORT, nullptr);
      break;
    }
    default: assert(("Invalid render dimensions.", false));
    }

    canvas.SwapBuffers();
    glutils::checkGLError();
  }
}