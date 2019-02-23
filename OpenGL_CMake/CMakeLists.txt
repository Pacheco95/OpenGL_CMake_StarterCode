# CMakeList.txt : CMake project for OpenGL_CMake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

SET(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")

include_directories(
	"${DEPENDENCIES_DIR}/GLEW/include"
	"${DEPENDENCIES_DIR}/GLFW/include"
)

# Add source to this project's executable.
add_executable (OpenGL_CMake "main.cpp")
set_target_properties(OpenGL_CMake PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC")

# TODO: Add tests and install targets if needed.
find_package(OpenGL REQUIRED)
IF(OpenGL_FOUND)
	target_link_libraries(OpenGL_CMake ${OPENGL_LIBRARIES})
ELSE()
	message("ERROR: OpenGL not found!")
ENDIF()


add_library(libglew STATIC IMPORTED)
set_property(TARGET libglew PROPERTY IMPORTED_LOCATION "${DEPENDENCIES_DIR}/GLEW/lib/glew32s.lib")
target_link_libraries(OpenGL_CMake libglew)

add_library(libglfw STATIC IMPORTED)
set_property(TARGET libglfw PROPERTY IMPORTED_LOCATION "${DEPENDENCIES_DIR}/GLFW/lib/glfw3.lib")
target_link_libraries(OpenGL_CMake libglfw)