target_sources (${PROJECT_NAME} PRIVATE
  GPU.cpp
  GPU.hpp
  GPUContext.cpp
  GPUContext.hpp
  Types.cpp
  Types.hpp)

add_subdirectory ("OpenCL")
add_subdirectory ("OpenGL")