# Troubleshooting Guide

This guide helps resolve common issues with the Hyprland 3D Carousel Plugin.

**WARNING: This plugin is in early development stages and is not ready for production use.**

## Quick Diagnostic Commands

Before troubleshooting, run these commands to gather information:

```bash
# Check Hyprland version
hyprctl version

# Check plugin status
hyprctl plugin list

# Check recent logs
journalctl -u hyprland.service --since "5 minutes ago"

# Verify plugin file
ls -la ~/.local/share/hyprland/plugins/hypr-carousel.so

# Check plugin symbols
nm -D ~/.local/share/hyprland/plugins/hypr-carousel.so | grep -E "(plugin|API)"
```

## Installation Issues

### Plugin Won't Load

#### Symptom
Plugin not appearing in `hyprctl plugin list` or Hyprland fails to start.

#### Possible Causes and Solutions

**1. Missing Plugin File**
```bash
# Check if plugin exists
ls -la ~/.local/share/hyprland/plugins/hypr-carousel.so
```
**Solution**: Reinstall plugin using `just install` or copy manually.

**2. Incorrect Path in Configuration**
```bash
# Check configuration file
grep -n "hypr-carousel" ~/.config/hypr/hyprland.conf
```
**Solution**: Ensure path matches actual plugin location.

**3. File Permissions**
```bash
# Check file permissions
ls -la ~/.local/share/hyprland/plugins/hypr-carousel.so
```
**Solution**: Make plugin executable:
```bash
chmod +x ~/.local/share/hyprland/plugins/hypr-carousel.so
```

**4. Version Mismatch**
```bash
# Check Hyprland version
hyprctl version
```
**Solution**: Ensure Hyprland version 0.50.0. Rebuild plugin if version differs.

### Plugin Loads But Crashes Hyprland

#### Symptom
Hyprland crashes immediately after loading plugin or when using carousel commands.

#### Diagnostic Steps

**1. Check Hyprland Logs**
```bash
journalctl -u hyprland.service --since "10 minutes ago" | grep -i "error\|crash\|fault"
```

**2. Verify Plugin Symbols**
```bash
nm -D ~/.local/share/hyprland/plugins/hypr-carousel.so | grep plugin
```
Expected output:
```
pluginAPIVersion
pluginInit  
pluginExit
```

**3. Check Dependencies**
```bash
ldd ~/.local/share/hyprland/plugins/hypr-carousel.so
```

#### Solutions

**1. ABI Version Mismatch**
- **Cause**: Plugin built for different Hyprland version
- **Solution**: Rebuild plugin ensuring exact version match:
```bash
cd /path/to/plugin
nix develop
just clean
just build
just install
```

**2. Incorrect Function Names**
- **Cause**: Plugin exports wrong function names
- **Solution**: Rebuild from latest source with correct camelCase names

**3. Missing Dependencies**
- **Cause**: Required libraries not found
- **Solution**: Install missing dependencies or use Nix environment

### "Missing apiver/init func" Error

#### Symptom
Error message about missing API version or init function.

#### Solution
This indicates incorrect function naming. The plugin must export:
- `pluginAPIVersion` (not `PLUGIN_API_VERSION`)
- `pluginInit` (not `PLUGIN_INIT`) 
- `pluginExit` (not `PLUGIN_EXIT`)

Rebuild plugin from latest source code.

## Runtime Issues

### Carousel Won't Activate

#### Symptom
`hyprctl dispatch carousel:toggle` has no effect.

#### Diagnostic Steps

**1. Check Plugin Status**
```bash
hyprctl plugin list | grep -i carousel
```

**2. Test Other Commands**
```bash
hyprctl dispatch carousel:next
hyprctl dispatch carousel:prev
```

**3. Check Logs**
```bash
journalctl -u hyprland.service --since "1 minute ago" | grep -i carousel
```

#### Solutions

**1. Plugin Not Loaded**
- **Solution**: Restart Hyprland or load plugin manually:
```bash
hyprctl plugin load ~/.local/share/hyprland/plugins/hypr-carousel.so
```

**2. No Active Workspaces**
- **Cause**: Carousel requires workspaces with windows
- **Solution**: Open some applications on different workspaces

**3. Initialization Failed**
- **Check**: Look for initialization errors in logs
- **Solution**: Restart Hyprland or reload plugin

### Visual Issues

#### Black Screen or Corrupted Display

**Possible Causes**:
1. **Graphics Driver Issues**
2. **OpenGL Compatibility Problems**  
3. **Texture Capture Failures**

**Solutions**:
1. **Update Graphics Drivers**
2. **Lower Texture Quality**:
   ```conf
   plugin {
       carousel {
           texture_quality = low
       }
   }
   ```
3. **Disable Visual Effects**:
   ```conf
   plugin {
       carousel {
           background_blur = false
           transparency_gradient = 0.0
       }
   }
   ```

#### Poor Performance or Stuttering

**Diagnostic**:
```bash
# Check GPU usage
nvidia-smi  # For NVIDIA
radeontop   # For AMD
```

**Solutions**:
1. **Reduce Quality Settings**:
   ```conf
   plugin {
       carousel {
           texture_quality = medium
           max_workspaces = 6
           animation_duration = 200
       }
   }
   ```

2. **Disable VSync**:
   ```conf
   plugin {
       carousel {
           vsync = false
       }
   }
   ```

3. **Close Unnecessary Applications**

#### Workspaces Not Displaying Correctly

**Symptoms**: Empty workspaces, wrong content, or missing applications

**Solutions**:
1. **Wait for Capture**: Allow time for workspace capture to complete
2. **Refresh Carousel**: Exit and re-enter carousel mode
3. **Check Workspace State**: Ensure workspaces have active windows

## Configuration Issues

### Configuration Not Applied

#### Symptom
Configuration changes in `hyprland.conf` don't take effect.

#### Solutions

**1. Syntax Errors**
```bash
# Check configuration syntax
hyprctl reload
```
Look for error messages about configuration parsing.

**2. Restart Hyprland**
Some configuration changes require a full restart.

**3. Dynamic Configuration Test**
Test settings dynamically:
```bash
hyprctl keyword plugin:carousel:radius 1000
hyprctl keyword plugin:carousel:animation_duration 500
```

### Invalid Configuration Values

#### Symptom
Plugin uses default values despite configuration.

#### Solutions

**1. Check Value Ranges**
Ensure all values are within valid ranges (see [Configuration Guide](configuration.md)).

**2. Check Option Names**
Verify exact spelling and case sensitivity:
```conf
# Correct
plugin {
    carousel {
        transparency_gradient = 0.3
    }
}

# Incorrect  
plugin {
    carousel {
        transparency_Gradient = 0.3  # Wrong case
        transparency-gradient = 0.3  # Wrong separator
    }
}
```

## Build Issues

### CMake Configuration Fails

#### Common Errors and Solutions

**1. "hyprland not found"**
```bash
# Check pkg-config path
echo $PKG_CONFIG_PATH

# For Nix environment
nix develop
```

**2. "No suitable C++ compiler"**
```bash
# Install compiler
sudo apt install build-essential  # Ubuntu
sudo pacman -S base-devel         # Arch
```

**3. Missing Dependencies**
```bash
# Use Nix environment (recommended)
nix develop

# Or install manually (system-dependent)
sudo apt install cmake ninja-build pkg-config
```

### Compilation Errors

#### "No such file or directory" for headers

**Cause**: Missing `hyprland/` prefix in includes
**Solution**: Ensure all includes use correct paths:
```cpp
// Correct
#include <hyprland/src/plugins/PluginAPI.hpp>

// Incorrect  
#include <src/plugins/PluginAPI.hpp>
```

#### Linking Errors

**Symptoms**: Undefined references, missing symbols
**Solutions**:
1. **Use Nix Environment**: `nix develop`
2. **Check Dependencies**: Ensure all required libraries are installed
3. **Clean Build**: `just clean && just build`

## Emergency Recovery

### Plugin Causes System Instability

**Immediate Actions**:
1. **Disable Plugin**:
   ```bash
   hyprctl plugin unload hypr-carousel
   ```

2. **Remove from Configuration**:
   Comment out plugin line in `hyprland.conf`:
   ```conf
   # plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so
   ```

3. **Restart Hyprland**: `hyprctl reload` or restart session

### Hyprland Won't Start

**Recovery Steps**:
1. **Use TTY**: Switch to TTY (Ctrl+Alt+F2)
2. **Edit Configuration**:
   ```bash
   nano ~/.config/hypr/hyprland.conf
   # Comment out plugin line
   ```
3. **Start Hyprland**: Return to desktop environment

### Complete Plugin Removal

```bash
# Remove plugin file
rm ~/.local/share/hyprland/plugins/hypr-carousel.so

# Remove configuration
sed -i '/hypr-carousel/d' ~/.config/hypr/hyprland.conf

# Restart Hyprland
hyprctl reload
```

## Debug Mode

### Enable Verbose Logging

Add to configuration:
```conf
plugin {
    carousel {
        debug_mode = true
        log_level = verbose
    }
}
```

### Collect Debug Information

```bash
# Create debug report
echo "=== System Information ===" > debug-report.txt
hyprctl version >> debug-report.txt
echo "=== Plugin Status ===" >> debug-report.txt
hyprctl plugin list >> debug-report.txt
echo "=== Recent Logs ===" >> debug-report.txt
journalctl -u hyprland.service --since "30 minutes ago" >> debug-report.txt
echo "=== Plugin Symbols ===" >> debug-report.txt
nm -D ~/.local/share/hyprland/plugins/hypr-carousel.so | grep plugin >> debug-report.txt
```

## Getting Help

### Before Reporting Issues

1. **Check this troubleshooting guide**
2. **Try minimal plugin** first: `just minimal && just test-load`
3. **Collect debug information** (see above)
4. **Test with default configuration**

### Information to Include

When reporting issues, include:
- Hyprland version (`hyprctl version`)
- Plugin build method (Nix/manual)
- Configuration file relevant sections
- Error messages from logs
- Steps to reproduce the issue

### Common Solutions Summary

| Issue | Quick Solution |
|-------|----------------|
| Plugin won't load | Check file path and permissions |
| Hyprland crashes | Rebuild with exact version match |
| Poor performance | Lower texture quality and max workspaces |
| Visual corruption | Update graphics drivers, disable effects |
| Configuration ignored | Check syntax, restart Hyprland |
| Build fails | Use Nix development environment |

For additional help, refer to:
- [Installation Guide](installation.md) for setup issues
- [Configuration Guide](configuration.md) for configuration problems
- [Development Guide](development.md) for build and development issues