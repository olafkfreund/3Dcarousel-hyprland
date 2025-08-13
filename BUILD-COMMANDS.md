# Build Commands for Testing

Since the current environment may not have all Hyprland dependencies available, here are the exact commands to run when you have a proper NixOS development environment:

## Nix Build Commands

```bash
# Enter the development environment
nix develop

# Or with direnv
direnv allow

# Build with Nix
nix build .#default

# Check build result
ls -la result/lib/
```

## Manual CMake Build (Minimal)

```bash
# Create minimal build directory
mkdir build-minimal
cp CMakeLists-minimal.txt build-minimal/CMakeLists.txt
cp -r src build-minimal/

# Configure and build
cd build-minimal
cmake . -G Ninja -DCMAKE_BUILD_TYPE=Debug
ninja

# Check result
ls -la hypr-carousel-minimal.so
```

## Manual CMake Build (Full Plugin)

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
ninja

# Check result
ls -la hypr-carousel.so
```

## Symbol Verification

```bash
# Check exported symbols (should show camelCase function names)
nm -D plugin.so | grep -E "(plugin|API)"

# Expected output:
# 00001234 T pluginAPIVersion
# 00005678 T pluginExit  
# 00009abc T pluginInit
```

## Dependency Check

```bash
# Check linked libraries
ldd plugin.so

# Should show Hyprland and system libraries
```

## Testing in Hyprland

```bash
# Load minimal plugin for testing
hyprctl plugin load $(pwd)/build-minimal/hypr-carousel-minimal.so

# Test dispatcher
hyprctl dispatch test:hello

# Unload plugin
hyprctl plugin unload hypr-carousel-minimal

# Load full plugin
hyprctl plugin load $(pwd)/build/hypr-carousel.so

# Test carousel
hyprctl dispatch carousel:toggle
```

## Environment Issues

If builds fail, likely causes:
1. **Missing Hyprland development headers** - Need `hyprland-dev` package
2. **Wrong Nix environment** - Must use `nix develop` or `direnv allow`
3. **Version mismatch** - Hyprland runtime must match build headers
4. **Missing pkg-config files** - Hyprland pkg-config must be in PATH

## Expected Results

### Successful Build
- `hypr-carousel-minimal.so` (~50KB)
- `hypr-carousel.so` (~200KB)
- No undefined symbols
- Proper camelCase function exports

### Successful Loading in Hyprland
- No crashes when loading plugin
- Notification appears: "[Test] Minimal plugin loaded!"
- Dispatcher works: `hyprctl dispatch test:hello`