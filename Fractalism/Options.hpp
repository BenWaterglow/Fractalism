#ifndef _FRACTALISM_OPTIONS_HPP_
#define _FRACTALISM_OPTIONS_HPP_

namespace fractalism {
  namespace options {
    enum class Dimensions : unsigned char {
      two,
      three
    };
    enum class RenderMode : unsigned char {
      escape,
      translated
    };
    enum class Space : unsigned char {
      phase = 1 << 0,
      dynamical = 1 << 1,
      both = phase | dynamical
    };
    enum class NumberSystem : unsigned char {
      c1,
      c2,
      c3,
      q,
      o
    };
    enum class ViewWindow : unsigned char {
      topLeft = 1 << 0,
      topRight = 1 << 1,
      bottomLeft = 1 << 2,
      bottomRight = 1 << 3
    };
  };
}
#endif