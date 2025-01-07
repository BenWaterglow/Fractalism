#ifndef _FRACTALISM_ARCBALL_CAMERA_HPP_
#define _FRACTALISM_ARCBALL_CAMERA_HPP_

#include <Fractalism/GPU/Types.hpp>

namespace fractalism::gpu::opengl {
      
  class ArcballCamera {
  public:
    ArcballCamera(real radius, real yaw, real pitch);
    ArcballCamera& operator+=(const types::Coordinates& delta);
    ArcballCamera& operator+=(real delta);
    types::vec3 getPosition() const;
    const types::mat4 createViewMatrix() const;
    types::mat4 createProjectionMatrix(real aspect) const;
  private:
    real radius;
    real yaw;
    real pitch;
    types::vec3 position;

    void updatePosition();
  };
}

#endif // end include guard