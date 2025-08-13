cmake_minimum_required(VERSION 3.16)
project(hypr-carousel VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

# Use pkg-config to find dependencies like official plugins
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
    hyprland
    pixman-1
    wayland-server
    wayland-client
    gl
    egl
    libdrm
    xkbcommon
)

add_library(hypr-carousel SHARED
    src/main.cpp
    src/CarouselRenderer.cpp
    src/WorkspaceManager.cpp
    src/AnimationEngine.cpp
)

target_include_directories(hypr-carousel PRIVATE include/)
target_link_libraries(hypr-carousel PRIVATE PkgConfig::deps)
target_compile_options(hypr-carousel PRIVATE -Wall -Wextra -fPIC)

# Set library properties (official pattern)
set_target_properties(hypr-carousel PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)