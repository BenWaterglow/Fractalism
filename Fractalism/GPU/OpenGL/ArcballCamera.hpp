#ifndef _FRACTALISM_ARCBALL_CAMERA_HPP_
#define _FRACTALISM_ARCBALL_CAMERA_HPP_

#include <Fractalism/GPU/Types.hpp>

namespace fractalism::gpu::opengl {

/**
 * @class ArcballCamera
 * @brief Represents an arcball camera for 3D navigation.
 */
class ArcballCamera {
public:
  /**
   * @brief Constructs an ArcballCamera with the specified radius, yaw, and
   * pitch.
   * @param radius The radius of the camera's orbit.
   * @param yaw The yaw angle of the camera.
   * @param pitch The pitch angle of the camera.
   */
  ArcballCamera(real radius, real yaw, real pitch);

  /**
   * @brief Moves the camera by the specified delta coordinates.
   * @param delta The delta coordinates to move the camera by.
   * @return Reference to this ArcballCamera.
   */
  ArcballCamera& operator+=(const types::Coordinates& delta);

  /**
   * @brief Zooms the camera by the specified delta.
   * @param delta The delta to zoom the camera by.
   * @return Reference to this ArcballCamera.
   */
  ArcballCamera& operator+=(real delta);

  /**
   * @brief Gets the position of the camera.
   * @return The position of the camera as a 3D vector.
   */
  types::vec3 getPosition() const;

  /**
   * @brief Creates the view matrix for the camera.
   * @return The view matrix.
   */
  types::mat4 createViewMatrix() const;

  /**
   * @brief Creates the projection matrix for the camera.
   * @param aspect The aspect ratio of the view.
   * @return The projection matrix.
   */
  types::mat4 createProjectionMatrix(real aspect) const;

private:
  real radius;          ///< The radius of the camera's orbit.
  real yaw;             ///< The yaw angle of the camera.
  real pitch;           ///< The pitch angle of the camera.
  types::vec3 position; ///< The position of the camera.

  /**
   * @brief Updates the position of the camera based on its radius, yaw, and
   * pitch.
   */
  void updatePosition();
};
} // namespace fractalism::gpu::opengl

#endif // end include guard
