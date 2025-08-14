cmake_minimum_required(VERSION 3.27)
project(hypr-carousel 
    VERSION 1.0.0
    DESCRIPTION "3D workspace carousel plugin for Hyprland"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

# Find dependencies using exact pattern from official plugins
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
    hyprland
    # Hyprland sub-dependencies
    hyprutils
    hyprlang
    hyprgraphics
    aquamarine
    # System dependencies
    libdrm
    libinput
    libudev
    pangocairo
    pixman-1
    wayland-server
    xkbcommon
)

# Use improved plugin following Hyprspace patterns
set(SRC_FILES
    src/main-improved.cpp
)
add_library(hypr-carousel SHARED ${SRC_FILES})

target_include_directories(hypr-carousel PRIVATE include/)
target_link_libraries(hypr-carousel PRIVATE PkgConfig::deps)
target_compile_options(hypr-carousel PRIVATE -Wall -Wextra -fPIC)

# Set library properties following official pattern
set_target_properties(hypr-carousel PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

# Install the plugin (official pattern)
install(TARGETS hypr-carousel LIBRARY)