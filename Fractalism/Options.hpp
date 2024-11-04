#ifndef _OPTIONS_HPP_
#define _OPTIONS_HPP_

namespace options {
  enum class Dimensions: unsigned short {
    two = 2,
    three = 3
  };
  enum class RenderMode {
    escape,
    translated
  };
  enum class Space: unsigned short {
    phase =     1 << 0,
    dynamical = 1 << 1,
    both = phase | dynamical
  };
  enum class NumberSystem: int {
    c1,
    c2,
    c3
  };
};

#endif