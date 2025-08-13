# Development Setup - Hyprland 3D Carousel Plugin

This project follows **official Hyprland plugin development patterns** and incorporates critical lessons learned from extensive debugging sessions.

## 🚨 Critical Development Guidelines

### Plugin Function Names (MUST BE CORRECT)
- ✅ **CORRECT**: `pluginAPIVersion()`, `pluginInit()`, `pluginExit()` (camelCase)  
- ❌ **WRONG**: `PLUGIN_API_VERSION()`, `PLUGIN_INIT()`, `PLUGIN_EXIT()`

### Header Paths (MUST USE hyprland/ PREFIX)
- ✅ **CORRECT**: `#include <hyprland/src/plugins/PluginAPI.hpp>`
- ❌ **WRONG**: `#include <src/plugins/PluginAPI.hpp>`

### Build System (MUST USE OFFICIAL PATTERN)
- ✅ **CORRECT**: CMake with `pkg-check_modules` and `PkgConfig::deps`
- ❌ **WRONG**: Manual g++ compilation or hardcoded paths

## NixOS Development Environment

This project provides a complete NixOS development environment using Nix flakes, pinned to **exact Hyprland 0.50.0 commit** for ABI compatibility.

### Prerequisites

1. **NixOS** with flakes enabled
2. **Hyprland 0.50.0** or later
3. **direnv** (optional, for automatic environment loading)

### Quick Start

```bash
# Clone and enter the project
cd 3Dcarousel-hyprland

# Option 1: Use direnv (recommended)
direnv allow  # Automatically loads Nix environment

# Option 2: Manual Nix development shell
nix develop

# Build the plugin
just build

# Install to Hyprland
just install
```

### Available Commands

The project uses [just](https://github.com/casey/just) as a command runner:

```bash
# Build commands
just build       # Build full plugin with CMake
just minimal     # Build minimal test plugin (recommended first)
just nix-build   # Build with Nix
just clean       # Clean build artifacts
just rebuild     # Clean and rebuild

# Testing & Debugging
just test-load   # Test minimal plugin loading and symbols
just test        # Run development tests
just debug       # Build with debug symbols

# Development
just dev         # Enter Nix development shell
just format      # Format code with clang-format
just lint        # Run static analysis
just check       # Run all checks

# Installation
just install     # Install plugin to ~/.local/share/hyprland/plugins/
just uninstall   # Remove plugin

# Testing
just test        # Run development tests
just debug       # Build with debug symbols
just info        # Show plugin information
```

### Development Dependencies

The Nix flake provides all necessary dependencies:

#### Hyprland 0.50.0 Stack
- `hyprland-dev` - Hyprland development headers
- `aquamarine` (≥0.9.2) - Rendering backend
- `hyprlang` (≥0.6.3) - Configuration language
- `hyprutils` (≥0.8.2) - Utility functions
- `hyprcursor` (≥0.1.13) - Cursor management
- `hyprgraphics` (≥0.1.5) - Graphics utilities

#### Graphics Dependencies
- `libGL` & `libEGL` - OpenGL support
- `wayland` & `wayland-protocols` - Wayland compositor support
- `pixman` - Pixel manipulation
- `libdrm` - Direct Rendering Manager
- `mesa` - OpenGL implementation

#### Build Tools
- `cmake` & `ninja` - Build system
- `pkg-config` - Package configuration
- `clang-tools` - Development tools
- `gdb` & `valgrind` - Debugging tools

### Project Structure

```
hypr-carousel/
├── flake.nix           # Nix development environment
├── justfile            # Command runner configuration
├── CMakeLists.txt      # Build configuration (official Hyprland pattern)
├── .envrc              # Direnv configuration
├── .gitignore          # Git ignore patterns
│
├── include/
│   └── plugin.hpp      # Main plugin interface
│
├── src/
│   ├── main.cpp        # Plugin entry points
│   ├── CarouselRenderer.{hpp,cpp}  # 3D rendering system
│   ├── WorkspaceManager.{hpp,cpp}  # Workspace navigation
│   └── AnimationEngine.{hpp,cpp}   # Animation system
│
└── build/              # Build artifacts (generated)
    └── hypr-carousel.so
```

### Configuration

Add to your `~/.config/hypr/hyprland.conf`:

```conf
# Load the plugin
plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so

# Keybindings
bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
bind = SUPER, right, exec, hyprctl dispatch carousel:next
bind = SUPER, left, exec, hyprctl dispatch carousel:prev
bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select
bind = SUPER, ESCAPE, exec, hyprctl dispatch carousel:exit

# Plugin configuration
plugin {
    carousel {
        radius = 800
        spacing = 1.2
        transparency_gradient = 0.3
        animation_duration = 300
    }
}
```

### Building Without Nix

If you prefer to build without Nix, ensure these dependencies are installed:

```bash
# NixOS
nix-shell -p cmake ninja pkg-config hyprland-dev wayland wayland-protocols \
  libGL libEGL libxkbcommon pixman libdrm mesa libinput pango cairo

# Then build normally
mkdir build && cd build
cmake .. -G Ninja
ninja
```

## Recommended Development Workflow

### 1. Start with Minimal Plugin
```bash
# ALWAYS test basic loading first
just minimal      # Build minimal test plugin
just test-load    # Verify symbols and dependencies
```

### 2. Test Loading in Hyprland
```bash
# Test the minimal plugin loads without crashing
hyprctl plugin load $(pwd)/build-minimal/hypr-carousel-minimal.so
hyprctl dispatch test:hello  # Test the dispatcher works
hyprctl plugin unload hypr-carousel-minimal
```

### 3. Build Full Plugin (Only After Minimal Works)
```bash
just build        # Build full plugin
just install      # Install to Hyprland
```

## Troubleshooting Plugin Crashes

### Common Issues and Solutions

#### 1. "missing apiver/init func" errors
- **Cause**: Wrong function names (common mistake)
- **Solution**: Use camelCase: `pluginAPIVersion()`, `pluginInit()`, `pluginExit()`
- **Check**: `nm plugin.so | grep plugin` should show correct names

#### 2. Plugin loads but crashes Hyprland
- **Cause**: ABI version mismatch
- **Solution**: Ensure exact Hyprland version matching (flake.nix pins to commit cb6589db)
- **Check**: Version in `hyprctl version` matches build headers

#### 3. "No such file or directory" for headers
- **Cause**: Missing `hyprland/` prefix in includes
- **Solution**: All includes must use `#include <hyprland/src/...>`

#### 4. Build errors with pkg-config
- **Cause**: Dependencies not found
- **Solution**: Enter Nix dev shell: `just dev` or `nix develop`

### Emergency Debugging Commands

```bash
# Check plugin symbols
nm -D plugin.so | grep -E "(plugin|API)"

# Check dependencies  
ldd plugin.so

# Check Hyprland logs
journalctl -u hyprland --since "5 minutes ago"

# Test minimal loading
hyprctl plugin load $(pwd)/build-minimal/hypr-carousel-minimal.so
```

### Contributing

1. Use the Nix development environment
2. Follow the existing code style
3. Run `just check` before committing
4. Test with `just test` and `just install`

### License

MIT License - see LICENSE file for details.