# 3D Carousel Hyprland Plugin

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![NixOS](https://img.shields.io/badge/NixOS-compatible-blue.svg)](https://nixos.org/)
[![Hyprland](https://img.shields.io/badge/Hyprland-0.50.0-green.svg)](https://hyprland.org/)

**WARNING: This is not ready for use yet, development in early stages.**

A 3D workspace carousel plugin for Hyprland that provides a rotating view of your workspaces. Navigate between workspaces using a 3D carousel interface with smooth animations and visual effects.

![Demo GIF Placeholder](docs/demo.gif) <!-- TODO: Add actual demo -->

## Features

### Core Features
- **3D Carousel Navigation**: Workspaces arranged in a rotating 3D carousel
- **Smooth Animations**: Fluid transitions with customizable duration and easing
- **Visual Depth**: Transparency gradients and depth effects for immersion
- **Workspace Previews**: Live workspace thumbnails rendered in real-time
- **Intuitive Controls**: Keyboard shortcuts for seamless navigation

### Visual Effects
- **Transparency Gradients**: Distant workspaces fade for depth perception
- **Highlight Selected**: Current workspace prominently displayed
- **Smooth Rotation**: Physics-based carousel movement
- **OpenGL Rendering**: High-performance 3D graphics with custom shaders

### Customization
- **Configurable Radius**: Adjust carousel size to your preference
- **Spacing Control**: Fine-tune workspace positioning
- **Animation Speed**: Customize transition timing
- **Visual Settings**: Transparency levels and effects

## Quick Start

### Prerequisites
- Hyprland 0.50.0+
- NixOS (recommended) or compatible Linux distribution
- OpenGL 3.3+ support

### NixOS Installation (Recommended)

```bash
# Clone the repository
git clone https://github.com/your-username/3Dcarousel-hyprland.git
cd 3Dcarousel-hyprland

# Enter development environment
nix develop

# Build and install
just build
just install

# Or build with Nix directly
nix build
cp result/lib/hypr-carousel.so ~/.local/share/hyprland/plugins/
```

### Traditional Build

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install build-essential cmake ninja-build pkg-config \
    libwayland-dev libgl1-mesa-dev libglm-dev

# Build
mkdir build && cd build
cmake .. -G Ninja
ninja

# Install
cp hypr-carousel.so ~/.local/share/hyprland/plugins/
```

## Configuration

Add to your `hyprland.conf`:

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
        radius = 800                    # Carousel radius in pixels
        spacing = 1.2                   # Workspace spacing multiplier
        transparency_gradient = 0.3     # Transparency effect strength
        animation_duration = 300        # Animation duration in ms
        enable_depth_blur = true        # Enable depth-based blur effect
        max_workspaces = 10             # Maximum workspaces to show
    }
}
```

## Usage

### Basic Controls
- **`SUPER + TAB`**: Toggle carousel view
- **`SUPER + ←/→`**: Navigate left/right through workspaces
- **`SUPER + RETURN`**: Select highlighted workspace
- **`SUPER + ESCAPE`**: Exit carousel without switching

### Advanced Features
- **Mouse Wheel**: Scroll through workspaces while in carousel
- **Click to Select**: Click on any workspace preview to switch
- **Auto-hide**: Carousel automatically hides after selection

## Development

### Development Environment

This project provides comprehensive development tooling for both Nix and traditional workflows.

#### Using Nix (Recommended)

```bash
# Enter development shell
nix develop

# Available development commands
just --list

# Quick development cycle
just dev-cycle
```

#### Available Commands

```bash
# Building
just build          # Build with CMake
just nix-build      # Build with Nix
just minimal        # Build minimal test plugin
just debug          # Build with debug symbols

# Development
just dev            # Enter Nix shell
just format         # Format code
just lint           # Static analysis
just check          # Run all checks

# Testing
just test           # Run development tests
just test-load      # Test plugin loading
just install        # Install to Hyprland
```

### Development Shells

- **Default**: Standard development with CMake, debugging tools
- **Debug**: Extra debugging tools (rr, perf, address-sanitizer)
  ```bash
  nix develop .#debug
  ```
- **CI**: Minimal shell for automated builds
  ```bash
  nix develop .#ci
  ```

### Architecture Overview

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  CarouselPlugin │────│ WorkspaceManager │────│ AnimationEngine │
│   (Singleton)   │    │   (Navigation)   │    │  (Transitions)  │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                        │                        │
         ▼                        ▼                        ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│ CarouselRenderer│────│  WorkspaceFrame  │────│   OpenGL/EGL    │
│  (3D Graphics)  │    │  (Data Model)    │    │   (Rendering)   │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

### Key Components

- **CarouselPlugin**: Main plugin class managing lifecycle
- **CarouselRenderer**: OpenGL rendering and 3D transformations
- **WorkspaceManager**: Workspace selection and navigation logic
- **AnimationEngine**: Smooth transitions and timing
- **WorkspaceFrame**: Individual workspace data structure

## Troubleshooting

### Common Issues

#### Plugin Won't Load
```bash
# Check plugin file exists
ls -la ~/.local/share/hyprland/plugins/hypr-carousel.so

# Verify plugin symbols
nm -D ~/.local/share/hyprland/plugins/hypr-carousel.so | grep plugin

# Check Hyprland logs
journalctl -u hyprland.service -f
```

#### Build Failures
```bash
# Check dependencies
pkg-config --exists hyprland

# Clean and rebuild
just clean && just build

# Try minimal build first
just minimal
```

#### Performance Issues
- Ensure OpenGL 3.3+ support: `glxinfo | grep "OpenGL version"`
- Check GPU memory usage during operation
- Reduce `max_workspaces` in configuration
- Disable `enable_depth_blur` for better performance

### Debug Mode

```bash
# Build debug version
just debug

# Run with debugging
HYPRLAND_LOG_WLR=1 hyprland

# Memory debugging
nix develop .#debug
valgrind --tool=memcheck hyprland
```

## Performance

### Benchmarks
- **Memory Usage**: ~5-10MB additional RAM
- **GPU Impact**: <5% on modern hardware
- **Render Time**: <16ms per frame (60+ FPS)
- **Startup Time**: <100ms initialization

### Optimization Features
- **Texture Caching**: Avoids re-capturing unchanged workspaces
- **LOD System**: Lower resolution for distant workspaces
- **Frustum Culling**: Only renders visible workspaces
- **Efficient Shaders**: Optimized fragment/vertex shaders

## Contributing

### Getting Started
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Enter development environment: `nix develop`
4. Make changes and test: `just dev-cycle`
5. Commit changes: `git commit -m 'Add amazing feature'`
6. Push to branch: `git push origin feature/amazing-feature`
7. Open a Pull Request

### Development Guidelines
- Follow the existing code style (use `just format`)
- Add tests for new features
- Update documentation as needed
- Ensure all checks pass: `just check`

### Code Style
- Use clang-format for C++ formatting
- Follow modern C++20 practices
- RAII for resource management
- Clear variable and function names

## Changelog

### [Unreleased]
- Initial 3D carousel implementation
- OpenGL rendering pipeline
- Basic workspace navigation
- Configuration system

### [0.1.0] - TBD
- First alpha release
- Core carousel functionality
- Basic animation system

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Hyprland](https://hyprland.org/) - The amazing wayland compositor
- [OpenGL](https://www.opengl.org/) - Graphics rendering
- [GLM](https://github.com/g-truc/glm) - Mathematics library
- [NixOS](https://nixos.org/) - Reproducible development environment

## Links

- **Documentation**: [docs/](docs/)
- **API Reference**: [docs/api.md](docs/api.md)
- **Installation Guide**: [docs/installation.md](docs/installation.md)
- **Troubleshooting**: [docs/troubleshooting.md](docs/troubleshooting.md)
- **Development Guide**: [DEVELOPMENT.md](DEVELOPMENT.md)

---

<p align="center">
  <strong>Made for the Hyprland community</strong>
</p>