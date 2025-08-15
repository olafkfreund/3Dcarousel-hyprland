cmake_minimum_required(VERSION 3.16)

project(hypr-carousel 
    VERSION 1.0.0
    DESCRIPTION "3D workspace carousel plugin for Hyprland"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(deps REQUIRED IMPORTED_TARGET
    hyprland
    libdrm
    pixman-1
    wayland-server
)

add_library(hypr-carousel SHARED src/main.cpp)

target_link_libraries(hypr-carousel PRIVATE PkgConfig::deps)
target_compile_options(hypr-carousel PRIVATE -Wall -Wextra -fPIC)

install(TARGETS hypr-carousel LIBRARY DESTINATION lib)