# Quick Start - Hyprland 3D Carousel Plugin

## 🚀 Rapid Development Setup

### 1. Essential Commands
```bash
# Enter development environment
direnv allow          # OR: nix develop

# Build and test minimal plugin FIRST
just minimal          # Build minimal test plugin
just test-load        # Verify symbols and dependencies

# Test in Hyprland
hyprctl plugin load $(pwd)/build-minimal/hypr-carousel-minimal.so
hyprctl dispatch test:hello    # Should show notification
hyprctl plugin unload hypr-carousel-minimal

# Build full plugin (only after minimal works)
just build
just install
```

### 2. Plugin Configuration
Add to `~/.config/hypr/hyprland.conf`:
```conf
plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so

bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
bind = SUPER, right, exec, hyprctl dispatch carousel:next
bind = SUPER, left, exec, hyprctl dispatch carousel:prev
bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select
```

## 🚨 Critical Requirements

### Function Names (MUST BE EXACT)
```cpp
extern "C" {
    APICALL EXPORT const char* pluginAPIVersion();    // camelCase!
    APICALL EXPORT PLUGIN_DESCRIPTION_INFO pluginInit(HANDLE handle);
    APICALL EXPORT void pluginExit();
}
```

### Header Includes (MUST USE PREFIX)
```cpp
#include <hyprland/src/plugins/PluginAPI.hpp>  // hyprland/ prefix!
#include <hyprland/src/Compositor.hpp>
```

### Version Pinning (MUST MATCH)
- Flake pinned to: `cb6589db98325705cef5dcaf92ccdf41ab21386d`
- Must match your Hyprland installation exactly

## ⚡ Emergency Debugging

```bash
# Plugin won't load - check symbols
nm -D plugin.so | grep plugin

# Hyprland crashes - check logs
journalctl -u hyprland --since "1 minute ago"

# Build fails - enter dev environment
nix develop
```

## 📋 Development Checklist

- [ ] Use `just minimal` first, never skip this step
- [ ] Test loading before building full plugin
- [ ] Check symbols with `nm` if loading fails
- [ ] Use camelCase function names
- [ ] Include `hyprland/` prefix in all headers
- [ ] Pin flake to exact Hyprland commit
- [ ] Test in Hyprland before declaring success

**Remember**: Start minimal, test loading, then add functionality!