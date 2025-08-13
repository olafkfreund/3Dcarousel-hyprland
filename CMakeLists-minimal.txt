cmake_minimum_required(VERSION 3.19)
project(hypr-carousel-minimal 
    VERSION 1.0.0
    DESCRIPTION "Minimal test plugin for Hyprland 0.50.0"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages using pkg-config (official Hyprland pattern)
find_package(PkgConfig REQUIRED)

# Essential dependencies for minimal plugin
pkg_check_modules(HYPRLAND REQUIRED hyprland>=0.50.0)
pkg_check_modules(PIXMAN REQUIRED pixman-1)
pkg_check_modules(WAYLAND_SERVER REQUIRED wayland-server)

# Minimal plugin - single source file
add_library(hypr-carousel-minimal SHARED
    src/main-minimal.cpp
)

# Include directories (using pkg-config paths)
target_include_directories(hypr-carousel-minimal PRIVATE
    ${HYPRLAND_INCLUDE_DIRS}
    ${PIXMAN_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
)

# Link libraries (official Hyprland pattern)
target_link_libraries(hypr-carousel-minimal
    ${HYPRLAND_LIBRARIES}
    ${PIXMAN_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
)

# Compile flags from pkg-config
target_compile_options(hypr-carousel-minimal PRIVATE
    ${HYPRLAND_CFLAGS_OTHER}
    ${PIXMAN_CFLAGS_OTHER}
    ${WAYLAND_SERVER_CFLAGS_OTHER}
)

# Add compiler definitions for Hyprland 0.50.0
target_compile_definitions(hypr-carousel-minimal PRIVATE
    HYPRLAND_VERSION="0.50.0"
    HYPRLAND_AVAILABLE
)

# Set library properties (official pattern)
set_target_properties(hypr-carousel-minimal PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

# Enable warnings and optimization
target_compile_options(hypr-carousel-minimal PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -fPIC
    "$<$<CONFIG:Release>:-O3;-DNDEBUG>"
    "$<$<CONFIG:Debug>:-g;-O0;-DDEBUG>"
)

# Development helper: show configuration
message(STATUS "=== Minimal Plugin Configuration ===")
message(STATUS "Plugin version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "======================================")