# Installation Guide - Hyprland 3D Carousel Plugin

This guide provides multiple installation methods for the Hyprland 3D Carousel Plugin, from the easiest NixOS module installation to manual compilation.

## Quick Installation (NixOS Users)

### Method 1: Using Flakes (Recommended)

Add the plugin to your NixOS configuration:

```nix
# flake.nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    hyprland.url = "github:hyprwm/Hyprland/v0.50.1";
    hypr-carousel = {
      url = "github:yourusername/3Dcarousel-hyprland";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.hyprland.follows = "hyprland";
    };
  };

  outputs = { nixpkgs, hyprland, hypr-carousel, ... }: {
    nixosConfigurations.yourhostname = nixpkgs.lib.nixosSystem {
      modules = [
        hyprland.nixosModules.default
        hypr-carousel.nixosModules.default
        {
          programs.hyprland.enable = true;
          
          # Enable and configure the 3D carousel plugin
          programs.hypr-carousel = {
            enable = true;
            
            # Plugin settings
            radius = 800;
            spacing = 1.2;
            transparencyGradient = 0.3;
            animationDuration = 300;
            enableDepthBlur = false;
            maxWorkspaces = 10;
            
            # Custom keybindings (optional)
            keybindings = {
              toggle = "SUPER, TAB";
              next = "SUPER, right";
              previous = "SUPER, left";
              select = "SUPER, RETURN";
              exit = "SUPER, ESCAPE";
            };
            
            # Additional configuration
            extraConfig = ''
              # Custom settings here
            '';
          };
        }
      ];
    };
  };
}
```

Apply the configuration:

```bash
sudo nixos-rebuild switch --flake .
```

### Method 2: Direct Plugin Installation

If you prefer to manage the plugin manually:

```nix
# configuration.nix or your NixOS module
{ config, pkgs, ... }:

let
  hypr-carousel = (builtins.getFlake "github:yourusername/3Dcarousel-hyprland").packages.${pkgs.system}.default;
in
{
  programs.hyprland = {
    enable = true;
    plugins = [ hypr-carousel ];
  };
}
```

Add to your Hyprland config:

```conf
# ~/.config/hypr/hyprland.conf
plugin {
    carousel {
        radius = 800
        spacing = 1.2
        transparency_gradient = 0.3
        animation_duration = 300
    }
}

# Keybindings
bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
bind = SUPER, right, exec, hyprctl dispatch carousel:next
bind = SUPER, left, exec, hyprctl dispatch carousel:prev
bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select
bind = SUPER, ESCAPE, exec, hyprctl dispatch carousel:exit
```

## Installation (Other Linux Distributions)

### Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake ninja-build pkg-config \
    libwayland-dev libwayland-client0 libwayland-server0 \
    libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev \
    libxkbcommon-dev libpixman-1-dev libdrm-dev \
    libinput-dev libxcb1-dev libxcb-composite0-dev \
    libglm-dev git
```

**Fedora:**
```bash
sudo dnf install gcc-c++ cmake ninja-build pkg-config \
    wayland-devel mesa-libGL-devel mesa-libGLES-devel \
    mesa-libEGL-devel libxkbcommon-devel pixman-devel \
    libdrm-devel libinput-devel glm-devel git
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake ninja pkg-config \
    wayland mesa libgl libxkbcommon pixman \
    libdrm libinput glm git
```

### Hyprland Development Headers

You need Hyprland development headers. This is distribution-specific:

**Arch Linux (AUR):**
```bash
yay -S hyprland-headers
# or
git clone https://aur.archlinux.org/hyprland-headers.git
cd hyprland-headers && makepkg -si
```

**Manual Hyprland Build:**
```bash
git clone --recursive https://github.com/hyprwm/Hyprland.git
cd Hyprland
git checkout v0.50.1
make all
sudo make install
```

### Building the Plugin

```bash
# Clone the repository
git clone https://github.com/yourusername/3Dcarousel-hyprland.git
cd 3Dcarousel-hyprland

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local

# Build the plugin
ninja

# Install the plugin
sudo ninja install
# or install to user directory:
cp hypr-carousel.so ~/.local/share/hyprland/plugins/
```

### Hyprland Configuration

Add to your `~/.config/hypr/hyprland.conf`:

```conf
# Load the plugin
plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so

# Plugin configuration
plugin {
    carousel {
        radius = 800
        spacing = 1.2
        transparency_gradient = 0.3
        animation_duration = 300
        enable_depth_blur = false
        max_workspaces = 10
    }
}

# Keybindings
bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
bind = SUPER, right, exec, hyprctl dispatch carousel:next
bind = SUPER, left, exec, hyprctl dispatch carousel:prev
bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select
bind = SUPER, ESCAPE, exec, hyprctl dispatch carousel:exit

# Debug keybinding (for troubleshooting)
bind = SUPER_CTRL, T, exec, hyprctl dispatch carousel:test
```

## Configuration Options

### Plugin Settings

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `radius` | int | 800 | Carousel radius in pixels (200-2000) |
| `spacing` | float | 1.2 | Workspace spacing multiplier (0.5-3.0) |
| `transparency_gradient` | float | 0.3 | Transparency effect strength (0.0-1.0) |
| `animation_duration` | int | 300 | Animation duration in milliseconds (50-1000) |
| `enable_depth_blur` | bool | false | Enable depth-based blur effect |
| `max_workspaces` | int | 10 | Maximum workspaces to display (3-20) |

### Advanced Configuration Examples

**Performance-focused (for older hardware):**
```conf
plugin {
    carousel {
        radius = 600
        spacing = 1.0
        transparency_gradient = 0.2
        animation_duration = 400
        enable_depth_blur = false
        max_workspaces = 8
    }
}
```

**Visual-focused (for modern hardware):**
```conf
plugin {
    carousel {
        radius = 1000
        spacing = 1.5
        transparency_gradient = 0.5
        animation_duration = 250
        enable_depth_blur = true
        max_workspaces = 12
    }
}
```

**Minimal setup:**
```conf
plugin {
    carousel {
        radius = 500
        spacing = 1.0
        transparency_gradient = 0.1
        animation_duration = 200
    }
}
```

## Verification

After installation, verify the plugin works:

1. **Reload Hyprland:**
   ```bash
   hyprctl reload
   ```

2. **Check plugin loading:**
   ```bash
   hyprctl plugins list
   ```

3. **Test compositor status:**
   ```bash
   hyprctl dispatch carousel:test
   ```

4. **Test carousel:**
   ```bash
   hyprctl dispatch carousel:toggle
   ```

## Troubleshooting

### Plugin Won't Load

**Check plugin file:**
```bash
ls -la ~/.local/share/hyprland/plugins/hypr-carousel.so
file ~/.local/share/hyprland/plugins/hypr-carousel.so
```

**Check symbols:**
```bash
nm -D ~/.local/share/hyprland/plugins/hypr-carousel.so | grep -i plugin
```

**Expected output:**
```
PLUGIN_API_VERSION
PLUGIN_INIT
PLUGIN_EXIT
```

### Build Errors

**Missing Hyprland headers:**
```bash
pkg-config --exists hyprland
pkg-config --cflags hyprland
```

**CMake can't find dependencies:**
```bash
export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

**Linker errors:**
```bash
# Check library dependencies
ldd hypr-carousel.so
```

### Runtime Issues

**Check Hyprland logs:**
```bash
journalctl -u hyprland -f
# or if running from terminal:
HYPRLAND_LOG_WLR=1 hyprland
```

**Test OpenGL support:**
```bash
glxinfo | grep "OpenGL version"
```

**Memory debugging:**
```bash
valgrind --tool=memcheck hyprland
```

## Uninstallation

### NixOS

Remove from your configuration and rebuild:

```nix
programs.hypr-carousel.enable = false;
```

### Manual Installation

```bash
rm ~/.local/share/hyprland/plugins/hypr-carousel.so
# Remove configuration from hyprland.conf
```

## Development Installation

For development and testing:

```bash
# Clone repository
git clone https://github.com/yourusername/3Dcarousel-hyprland.git
cd 3Dcarousel-hyprland

# NixOS development environment
nix develop

# Build debug version
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja
ninja

# Install development version
cp hypr-carousel.so ~/.local/share/hyprland/plugins/hypr-carousel-dev.so
```

Update your Hyprland config to use the development version:
```conf
plugin = ~/.local/share/hyprland/plugins/hypr-carousel-dev.so
```

## Performance Tuning

### For Lower-End Hardware

- Set `radius = 400-600`
- Use `animation_duration = 400-500`
- Set `transparency_gradient = 0.1-0.2`
- Disable `enable_depth_blur`
- Limit `max_workspaces = 6-8`

### For High-End Hardware

- Set `radius = 1000-1200`
- Use `animation_duration = 200-250`
- Set `transparency_gradient = 0.4-0.6`
- Enable `enable_depth_blur`
- Allow `max_workspaces = 12-15`

### System Requirements

**Minimum:**
- CPU: Any x86_64 processor from the last 10 years
- GPU: OpenGL 3.3 support (Intel HD 4000+, AMD HD 7000+, NVIDIA GTX 400+)
- RAM: 4GB total system memory
- Hyprland: Version 0.50.0+

**Recommended:**
- CPU: Modern multi-core processor
- GPU: Dedicated graphics card with 1GB+ VRAM
- RAM: 8GB+ total system memory
- Hyprland: Version 0.50.1+

---

For support or questions, please check the [README](README.md) or open an issue on GitHub.