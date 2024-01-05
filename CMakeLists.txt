cmake_minimum_required(VERSION 3.5)
project(confetti_exploder)

# Set the source files for your plugin
set(SOURCE_FILES
    confetti_exploder.cpp
)

# Find required packages (GLFW and GLEW)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)

# Find OBS SDK
find_path(OBS_INCLUDE_DIR NAMES obs.h HINTS /usr/include/obs)
include_directories(${OBS_INCLUDE_DIR})

# Include directories for GLFW and GLEW
include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

# Create the shared library
add_library(${PROJECT_NAME} MODULE ${SOURCE_FILES})

# Set the C++ standard to C++11
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# Link against OBS libraries, GLFW, and GLEW
target_link_libraries(${PROJECT_NAME} obs ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})

# Set installation path for the plugin
install(TARGETS ${PROJECT_NAME} DESTINATION ~/.config/obs-studio/plugins)

