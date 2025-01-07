#include <Fractalism/GPU/OpenGL/ArcballCamera.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace fractalism::gpu::opengl {

  namespace {
    static constexpr const float zNear = 0.1f;
    static constexpr const float zFar = 100.0f;
    static constexpr const float fov = 45.0f;
  }

  ArcballCamera::ArcballCamera(real radius, real yaw, real pitch) :
        radius(radius),
        yaw(yaw),
        pitch(pitch) {
    updatePosition();
  }

  ArcballCamera& ArcballCamera::operator+=(const types::Coordinates& delta) {
    constexpr real sensitivity = 1.0;
    constexpr real nearPoles = glm::half_pi<real>() - 0.1;
    yaw += delta.x * sensitivity;
    // Clamp pitch to avoid gimbal lock
    pitch = glm::clamp(pitch - delta.y * sensitivity, -nearPoles, nearPoles);
    updatePosition();
    return *this;
  }

  ArcballCamera& ArcballCamera::operator+=(const real delta) {
    constexpr real zoomSpeed = 0.1;
    radius -= delta * zoomSpeed;
    // Avoid negative or zero radius
    radius = glm::max(radius, 0.1);
    updatePosition();
    return *this;
  }

  types::vec3 ArcballCamera::getPosition() const {
    return position;
  }

  const types::mat4 ArcballCamera::createViewMatrix() const {
    return glm::lookAt(
        position,
        types::vec3{ 0.0, 0.0, 0.0 },
        types::vec3{ 0.0, 1.0, 0.0 });
  }

  types::mat4 ArcballCamera::createProjectionMatrix(real aspect) const {
    static constexpr const real zNear = 0.1;
    static constexpr const real zFar = 100.0;
    static constexpr const real fov = glm::radians(45.0);
    return glm::perspective(fov, aspect, zNear, zFar);
  }
  
  void ArcballCamera::updatePosition() {
    position = types::vec3(
      -radius * cos(pitch) * sin(yaw),
      -radius * sin(pitch),
      -radius * cos(pitch) * cos(yaw));
  }
}