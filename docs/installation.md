# Installation Guide

This guide covers installing the Hyprland 3D Carousel Plugin on various systems.

**WARNING: This plugin is in early development stages and is not ready for production use.**

## Prerequisites

### System Requirements
- **Hyprland 0.50.0 or later** (exact version matching required)
- **OpenGL 3.3+ support**
- **NixOS** (recommended) or compatible Linux distribution
- **C++23 compatible compiler** (GCC 10+ or Clang 12+)
- **CMake 3.19+**

### Hyprland Version Compatibility
This plugin is built specifically for Hyprland 0.50.0 (commit `cb6589db98325705cef5dcaf92ccdf41ab21386d`). Using different versions may cause crashes or instability.

Check your Hyprland version:
```bash
hyprctl version
```

## Installation Methods

### Method 1: NixOS with Flakes (Recommended)

This is the preferred installation method as it ensures exact dependency versions and ABI compatibility.

#### Prerequisites
- NixOS with flakes enabled
- Git

#### Installation Steps

1. **Clone the repository**:
```bash
git clone <repository-url>
cd 3Dcarousel-hyprland
```

2. **Enter the development environment**:
```bash
# Option A: Using direnv (recommended)
direnv allow

# Option B: Manual Nix development shell
nix develop
```

3. **Build the minimal test plugin first** (critical for safety):
```bash
just minimal
just test-load
```

4. **Test minimal plugin loading**:
```bash
hyprctl plugin load $(pwd)/build-minimal/hypr-carousel-minimal.so
hyprctl dispatch test:hello
hyprctl plugin unload hypr-carousel-minimal
```

5. **Build the full plugin** (only after minimal test succeeds):
```bash
just build
```

6. **Install the plugin**:
```bash
just install
```

### Method 2: Manual Build with Nix Package Manager

If you have Nix package manager on a non-NixOS system:

1. **Install Nix** (if not already installed):
```bash
curl -L https://nixos.org/nix/install | sh
```

2. **Enable flakes**:
```bash
mkdir -p ~/.config/nix
echo "experimental-features = nix-command flakes" >> ~/.config/nix/nix.conf
```

3. **Follow the NixOS installation steps above**.

### Method 3: Traditional Build (Advanced Users)

**Warning**: This method requires manual dependency management and version matching.

#### Dependencies
Install these packages on your system:
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake pkg-config libwayland-dev libxkbcommon-dev \
    libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdrm-dev libpixman-1-dev

# Arch Linux  
sudo pacman -S base-devel cmake wayland wayland-protocols libxkbcommon mesa libdrm pixman

# Fedora
sudo dnf install gcc-c++ cmake wayland-devel libxkbcommon-devel mesa-libGL-devel \
    mesa-libEGL-devel libdrm-devel pixman-devel
```

#### Hyprland Development Headers
You must install Hyprland development headers that exactly match your runtime version:

```bash
# This varies by distribution - ensure version 0.50.0
# Example for package managers that support it:
sudo pacman -S hyprland-dev  # Arch
```

#### Build Steps
```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

## Installation Verification

### Check Plugin File
After building, verify the plugin exists:
```bash
ls -la build/hypr-carousel.so
# OR for Nix build:
ls -la result/lib/hypr-carousel.so
```

### Verify Symbols
Check that the plugin exports the correct functions:
```bash
nm -D build/hypr-carousel.so | grep -E "(plugin|API)"
```

Expected output should show:
```
pluginAPIVersion
pluginInit
pluginExit
```

### Check Dependencies
Verify all required libraries are linked:
```bash
ldd build/hypr-carousel.so
```

## Plugin Installation

### Automatic Installation (Recommended)
```bash
just install
```

This copies the plugin to `~/.local/share/hyprland/plugins/hypr-carousel.so`.

### Manual Installation
```bash
mkdir -p ~/.local/share/hyprland/plugins
cp build/hypr-carousel.so ~/.local/share/hyprland/plugins/
```

## Hyprland Configuration

Add the plugin to your Hyprland configuration file (`~/.config/hypr/hyprland.conf`):

```conf
# Load the plugin
plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so
```

## Loading the Plugin

### Method 1: Restart Hyprland
After adding the plugin to your configuration, restart Hyprland.

### Method 2: Dynamic Loading (for testing)
```bash
hyprctl plugin load ~/.local/share/hyprland/plugins/hypr-carousel.so
```

## Verification

### Check Plugin Status
```bash
hyprctl plugin list
```

You should see "Hypr Carousel" in the list of loaded plugins.

### Test Basic Functionality
```bash
hyprctl dispatch carousel:toggle
```

If successful, you should see the 3D carousel activate.

## Uninstallation

### Remove Plugin File
```bash
just uninstall
# OR manually:
rm ~/.local/share/hyprland/plugins/hypr-carousel.so
```

### Remove from Configuration
Remove or comment out the plugin line in `hyprland.conf`:
```conf
# plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so
```

### Dynamic Unloading
```bash
hyprctl plugin unload hypr-carousel
```

## Common Installation Issues

### Plugin Won't Load
1. **Check Hyprland version compatibility**
2. **Verify plugin file permissions**
3. **Check Hyprland logs**: `journalctl -u hyprland.service -f`

### Build Failures
1. **Missing dependencies**: Install all required packages
2. **Version mismatch**: Ensure Hyprland headers match runtime version
3. **Compilation errors**: Use Nix development environment

### Symbol Errors
1. **Wrong function names**: Rebuild with latest code
2. **Missing symbols**: Check dependency installation

For detailed troubleshooting, see the [Troubleshooting Guide](troubleshooting.md).

## Next Steps

After successful installation:
1. **Configure the plugin**: See [Configuration Guide](configuration.md)
2. **Learn the features**: See [Usage Guide](usage.md)
3. **Set up keybindings**: See [Keybindings Reference](keybindings.md)