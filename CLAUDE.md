# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Hyprland plugin that creates a 3D carousel effect for workspace switching. The plugin renders workspaces in a rotating carousel with depth, transparency effects, and smooth animations using OpenGL.

## Architecture

### Core Components

- **CarouselPlugin**: Main singleton plugin class that manages the entire lifecycle
- **CarouselRenderer**: Handles OpenGL rendering, shaders, and 3D transformations
- **WorkspaceManager**: Manages workspace selection and navigation logic
- **AnimationEngine**: Handles smooth transitions and animations
- **WorkspaceFrame**: Data structure for individual workspace representations

### Key Technologies

- **Language**: C++20
- **Graphics**: OpenGL 3.3+ with custom vertex/fragment shaders
- **Math Library**: GLM for matrix transformations
- **Platform**: Hyprland window manager plugin system
- **Build System**: CMake 3.19+

### Plugin System Integration

The plugin uses Hyprland's plugin API with these entry points:

- `PLUGIN_INIT()`: Initialize and register dispatchers
- `PLUGIN_EXIT()`: Cleanup resources
- Custom dispatchers: `carousel:toggle`, `carousel:next`, `carousel:prev`, `carousel:select`, `carousel:exit`

## Development Commands

### Building the Plugin

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build (use all available cores)
make -j$(nproc)
```

### Installation

```bash
# Install to Hyprland plugin directory
cp hypr-carousel.so ~/.local/share/hyprland/plugins/
```

### Testing and Debugging

```bash
# Enable debug logging
export HYPRLAND_LOG_WLR=1
export HYPRLAND_NO_RT=1

# Restart Hyprland to load plugin
hyprctl reload
```

## Code Structure and Patterns

### File Organization

The project follows a standard C++ structure:

- `include/plugin.hpp`: Main plugin interface and declarations
- `src/main.cpp`: Plugin entry points and initialization
- `src/CarouselRenderer.cpp/.hpp`: OpenGL rendering implementation
- `src/WorkspaceManager.cpp/.hpp`: Workspace management logic
- `src/AnimationEngine.cpp/.hpp`: Animation system
- `CMakeLists.txt`: Build configuration

### OpenGL Rendering Pipeline

1. **Shader Creation**: Vertex and fragment shaders compiled at initialization
2. **Geometry Setup**: Quad vertices for workspace rendering with VAO/VBO/EBO
3. **Workspace Capture**: Render workspaces to textures (framebuffers)
4. **3D Transformation**: Use GLM matrices for projection, view, and model transforms
5. **Carousel Rendering**: Position workspaces in 3D space with transparency

### Key Design Patterns

- **Singleton Pattern**: CarouselPlugin ensures single instance
- **RAII**: Automatic OpenGL resource cleanup in destructors
- **Component Architecture**: Separate concerns into distinct classes

## Dependencies and Requirements

### System Dependencies

```bash
# Ubuntu/Debian
sudo apt install build-essential cmake pkg-config libwayland-dev libxkbcommon-dev \
    libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdrm-dev libxcb1-dev \
    libxcb-composite0-dev libxcb-xfixes0-dev libxcb-xinput-dev libxcb-image0-dev \
    libxcb-shm0-dev libxcb-util0-dev libxcb-keysyms1-dev libxcb-randr0-dev \
    libxcb-ewmh-dev libpixman-1-dev libinput-dev

# Arch Linux
sudo pacman -S base-devel cmake wayland wayland-protocols libxkbcommon \
    mesa libdrm libxcb xcb-util xcb-util-keysyms xcb-util-wm pixman libinput
```

### Required Libraries (CMake)

- Hyprland development headers
- OpenGL/EGL
- Wayland client/server
- GLM (for matrix math)

## Configuration

### Hyprland Configuration

Add to `hyprland.conf`:

```conf
# Load plugin
plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so

# Keybindings
bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
bind = SUPER, right, exec, hyprctl dispatch carousel:next
bind = SUPER, left, exec, hyprctl dispatch carousel:prev
bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select
bind = SUPER, ESCAPE, exec, hyprctl dispatch carousel:exit

# Plugin settings
plugin {
    carousel {
        radius = 800
        spacing = 1.2
        transparency_gradient = 0.3
        animation_duration = 300
    }
}
```

## OpenGL Shader Details

### Vertex Shader Features

- 3D position transformations with MVP matrix
- Texture coordinate mapping
- OpenGL 3.3 core profile compatibility

### Fragment Shader Features

- Texture sampling with transparency
- Selected workspace highlighting
- Alpha blending for depth effect

## Common Development Tasks

### Adding New Animation Effects

1. Extend `AnimationEngine` class with new animation functions
2. Add timing and easing parameters
3. Update render loop to apply transformations
4. Test with different workspace configurations

### Modifying 3D Layout

1. Adjust carousel radius and spacing in `CarouselRenderer`
2. Modify angle calculations in `updateWorkspacePositions()`
3. Update projection matrix parameters for field of view
4. Test with different numbers of workspaces

### Performance Optimization

1. **Texture Caching**: Avoid re-capturing unchanged workspaces
2. **LOD System**: Use lower resolution textures for distant workspaces
3. **Frustum Culling**: Don't render workspaces outside view
4. **Shader Optimization**: Minimize fragment shader complexity

## Troubleshooting

### Plugin Loading Issues

- Verify Hyprland version compatibility with plugin API
- Check that all dependencies are installed and found by CMake
- Ensure plugin path is correct in Hyprland configuration

### Rendering Problems

- Check OpenGL context creation and shader compilation logs
- Verify graphics driver support for OpenGL 3.3+
- Test with debug logging enabled

### Performance Issues

- Monitor GPU usage and memory consumption
- Profile texture memory usage
- Check for OpenGL state leaks or redundant calls

## Current Implementation Status

This repository contains detailed documentation and complete code examples for the plugin, but the actual source files need to be created following the specifications in `hyprland-carousel-plugin.md`. The documentation provides a comprehensive blueprint for implementation.

- "create a justfile for build and testing. also add nix build and nix developt with a flake.nix since we are running on nixos"
- "The official Hyprland plugins use CMake with pkg-config, not manual header paths"

Hyprland Plugin Crash Resolution: Critical Debugging Session\*\*

**Problem**: Plugin would compile successfully but crash Hyprland on load
**Root Cause Analysis**: Multiple interrelated issues discovered through systematic debugging

**Key Issues Identified**:

1. **Function Naming Convention**:
   - **Wrong**: Used `PLUGIN_API_VERSION()`, `PLUGIN_INIT()`, `PLUGIN_EXIT()`
   - **Correct**: Hyprland expects `pluginAPIVersion()`, `pluginInit()`, `pluginExit()` (camelCase)
   - **Detection**: Symbol inspection with `nm plugin.so` revealed correct export names

2. **Header Include Paths**:
   - **Wrong**: `#include <src/plugins/PluginAPI.hpp>`
   - **Correct**: `#include <hyprland/src/plugins/PluginAPI.hpp>` for Nix environments
   - **Impact**: Compilation failures and missing symbol definitions

3. **Build System Approach**:
   - **Wrong**: Manual Makefile with hardcoded compilation flags
   - **Correct**: CMake with pkg-config following official plugin patterns
   - **Research Source**: Official Hyprland plugins repository structure analysis

**Resolution Process**:

1. **Research Phase**: Analyzed official Hyprland plugins repository for build patterns
2. **Minimal Testing**: Created ultra-minimal plugins to isolate crash causes
3. **Symbol Analysis**: Used `nm` and `objdump` to verify correct symbol exports
4. **Build System Migration**: Switched to CMake with proper pkg-config integration
5. **Version Pinning**: Ensured exact header/runtime version matching

**Critical Research Findings**:

- Official plugins ALL use CMake with pkg-config for dependency resolution
- Function names must match exact camelCase format expected by plugin loader
- Nix development headers require `hyprland/` prefix for proper resolution
- ABI compatibility requires exact version matching between build and runtime

**Best Practice for Plugin Crashes**:

1. **Start with minimal plugin** - test basic loading before adding functionality
2. **Verify symbol exports** - use `nm plugin.so | grep plugin` to check names
3. **Follow official patterns** - research actual plugin repositories, not documentation
4. **Use CMake + pkg-config** - don't try to manually manage compilation flags
5. **Test incrementally** - add functionality only after basic loading works

**Reference Guide Created**:
`CLAUDE-HYPRLAND-PLUGIN-DEVELOPMENT.md` - Comprehensive guide for all future Hyprland plugin development, incorporating all lessons learned from this debugging session.

**Time Investment vs. Value**:
Extensive debugging session (multiple attempts over several iterations) that resulted in fundamental understanding of Hyprland plugin architecture. This knowledge is now documented for reuse across projects.

This debugging experience highlights the importance of:

- Following official project patterns rather than trying to create custom approaches
- Systematic troubleshooting (minimal test cases, symbol verification, incremental testing)
- Research-driven development (analyzing working examples rather than just documentation)
- Creating reusable knowledge artifacts (comprehensive guides) from complex debugging sessions

The resolution demonstrates that plugin crashes are often caused by fundamental build system or API contract issues rather than code logic problems.

