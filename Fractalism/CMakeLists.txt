# CMakeList.txt : CMake project for Fractalism, include source and define
# project specific logic here.
#

find_package(wxWidgets 3.2.6 CONFIG REQUIRED)
find_package(GLEW 2.1.0 REQUIRED)
find_package(glm 1.0.1 CONFIG REQUIRED)
find_package(OpenCL 2.0 CONFIG REQUIRED)

if (WIN32)
    set(ADDITIONAL_EXECUTABLE_ARGS WIN32)
else (WIN32)
    set(ADDITIONAL_EXECUTABLE_ARGS "")
endif (WIN32)

add_executable (${PROJECT_NAME} ${ADDITIONAL_EXECUTABLE_ARGS})

target_sources(${PROJECT_NAME} PRIVATE
    KernelHeaders/cltypes.h
    KernelHeaders/interop.h
    KernelHeaders/kernels.h
    KernelHeaders/number_systems.h
    KernelHeaders/spectral_color.h
    App.cpp
    App.hpp
    Events.cpp
    Events.hpp
    Exceptions.cpp
    Exceptions.hpp
    Options.hpp
    Proxy.hpp
    Settings.cpp
    Settings.hpp
    Utils.cpp
    Utils.hpp)

add_subdirectory("GPU")
add_subdirectory("UI")

target_link_libraries(${PROJECT_NAME} PRIVATE wx::wxcore wx::wxbase wx::wxgl GLEW::glew OpenCL::OpenCL)
target_include_directories(${PROJECT_NAME} PRIVATE glm::glm-header-only ${PROJECT_SOURCE_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE USE_DOUBLE_MATH MAX_NUMBER_SYSTEM_SIZE=8)

add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
    COMMENT "Copying shaders")

add_custom_target(copy_kernels
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/KernelHeaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/KernelHeaders
    COMMENT "Copying kernel headers")

add_dependencies(${PROJECT_NAME} copy_shaders copy_kernels)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.
