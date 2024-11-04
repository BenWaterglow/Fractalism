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

add_executable (${PROJECT_NAME} ${ADDITIONAL_EXECUTABLE_ARGS}
    CLIncludeHelper.hpp
	CLSolver.cpp
    CLSolver.hpp
    CLTypes.hpp
    CLUtils.cpp
    CLUtils.hpp
    Events.cpp
    Events.hpp
    Exceptions.cpp
    Exceptions.hpp
    FractalismApp.cpp
    FractalismApp.hpp
    GLRenderCanvas.cpp
    GLRenderCanvas.hpp
    GLRenderer.cpp
    GLRenderer.hpp
    GLShader.cpp
    GLShader.hpp
    GLShaderProgram.cpp
    GLShaderProgram.hpp
    GLTexture3D.cpp
    GLTexture3D.hpp
    GLUtils.cpp
    GLUtils.hpp
    GPUContext.cpp
    GPUContext.hpp
    MainUIFrame.cpp
    MainUIFrame.hpp
    NumberInput.cpp
    NumberInput.hpp
    Options.hpp
    RenderPanel.cpp
    RenderPanel.hpp
    Settings.hpp
    SharedState.cpp
    SharedState.hpp
    Utils.cpp
    Utils.hpp
    ViewMappingSelector.cpp
    ViewMappingSelector.hpp
    WxIncludeHelper.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE wx::wxcore wx::wxbase wx::wxgl GLEW::glew OpenCL::OpenCL)
target_include_directories(${PROJECT_NAME} PRIVATE glm::glm-header-only)

add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
    COMMENT "Copying shaders")

add_custom_target(copy_kernels
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Kernels
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Kernels
    COMMENT "Copying kernels")

add_dependencies(${PROJECT_NAME} copy_shaders copy_kernels)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
