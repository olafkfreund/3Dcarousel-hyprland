# Configuration Guide

This guide covers all configuration options for the Hyprland 3D Carousel Plugin.

**WARNING: This plugin is in early development stages and is not ready for production use.**

## Basic Configuration

### Loading the Plugin

Add this line to your `~/.config/hypr/hyprland.conf`:

```conf
plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so
```

### Basic Keybindings

Add these keybindings to enable basic carousel functionality:

```conf
# Essential carousel keybindings
bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
bind = SUPER, right, exec, hyprctl dispatch carousel:next  
bind = SUPER, left, exec, hyprctl dispatch carousel:prev
bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select
bind = SUPER, ESCAPE, exec, hyprctl dispatch carousel:exit
```

## Plugin Configuration Options

### Plugin Configuration Block

Add a plugin configuration block to your `hyprland.conf`:

```conf
plugin {
    carousel {
        # 3D carousel settings
        radius = 800
        spacing = 1.2
        transparency_gradient = 0.3
        animation_duration = 300
        
        # Visual settings
        workspace_scale = 0.8
        selected_scale = 1.0
        background_blur = true
        show_workspace_names = true
        
        # Performance settings
        max_workspaces = 10
        texture_quality = high
        vsync = true
    }
}
```

## Configuration Options Reference

### 3D Carousel Settings

#### `radius = <number>`
- **Default**: `800`
- **Range**: `400-1500`
- **Description**: Distance of workspaces from the center of the carousel
- **Example**: `radius = 1000` for a larger carousel

#### `spacing = <float>`
- **Default**: `1.2`  
- **Range**: `0.5-3.0`
- **Description**: Angular spacing between workspaces (multiplier)
- **Example**: `spacing = 2.0` for more space between workspaces

#### `transparency_gradient = <float>`
- **Default**: `0.3`
- **Range**: `0.0-1.0`
- **Description**: How much non-selected workspaces fade (0 = no fade, 1 = maximum fade)
- **Example**: `transparency_gradient = 0.5` for more pronounced fading

#### `animation_duration = <milliseconds>`
- **Default**: `300`
- **Range**: `100-1000`
- **Description**: Duration of carousel rotation animations
- **Example**: `animation_duration = 500` for slower animations

### Visual Settings

#### `workspace_scale = <float>`
- **Default**: `0.8`
- **Range**: `0.3-1.0`
- **Description**: Scale of non-selected workspaces
- **Example**: `workspace_scale = 0.6` for smaller inactive workspaces

#### `selected_scale = <float>`
- **Default**: `1.0`
- **Range**: `0.8-1.5`
- **Description**: Scale of the currently selected workspace
- **Example**: `selected_scale = 1.2` for emphasized selection

#### `background_blur = <boolean>`
- **Default**: `true`
- **Values**: `true`, `false`
- **Description**: Enable background blur effect when carousel is active
- **Example**: `background_blur = false` to disable blur

#### `show_workspace_names = <boolean>`
- **Default**: `true`
- **Values**: `true`, `false`
- **Description**: Display workspace names/numbers in the carousel
- **Example**: `show_workspace_names = false` to hide names

### Performance Settings

#### `max_workspaces = <number>`
- **Default**: `10`
- **Range**: `3-20`
- **Description**: Maximum number of workspaces to include in carousel
- **Example**: `max_workspaces = 15` for more workspaces

#### `texture_quality = <quality>`
- **Default**: `high`
- **Values**: `low`, `medium`, `high`, `ultra`
- **Description**: Quality of workspace texture capture
- **Example**: `texture_quality = medium` for better performance

#### `vsync = <boolean>`
- **Default**: `true`
- **Values**: `true`, `false`
- **Description**: Enable vertical sync for smoother animations
- **Example**: `vsync = false` for potentially higher FPS

## Advanced Configuration

### Custom Keybinding Schemes

#### Vim-style Navigation
```conf
bind = SUPER, h, exec, hyprctl dispatch carousel:prev
bind = SUPER, l, exec, hyprctl dispatch carousel:next
bind = SUPER, k, exec, hyprctl dispatch carousel:toggle
bind = SUPER, j, exec, hyprctl dispatch carousel:select
```

#### Arrow Key Navigation
```conf
bind = SUPER_SHIFT, left, exec, hyprctl dispatch carousel:prev
bind = SUPER_SHIFT, right, exec, hyprctl dispatch carousel:next
bind = SUPER_SHIFT, up, exec, hyprctl dispatch carousel:toggle
bind = SUPER_SHIFT, down, exec, hyprctl dispatch carousel:select
```

#### Mouse Integration
```conf
# Mouse wheel navigation (when carousel is active)
bind = SUPER, mouse_wheel_up, exec, hyprctl dispatch carousel:prev
bind = SUPER, mouse_wheel_down, exec, hyprctl dispatch carousel:next
bind = SUPER, mouse:272, exec, hyprctl dispatch carousel:select
```

### Conditional Configuration

#### High-Performance Systems
```conf
plugin {
    carousel {
        radius = 1200
        animation_duration = 200
        texture_quality = ultra
        max_workspaces = 15
        transparency_gradient = 0.5
    }
}
```

#### Low-Performance Systems
```conf
plugin {
    carousel {
        radius = 600
        animation_duration = 500
        texture_quality = low
        max_workspaces = 6
        transparency_gradient = 0.2
        background_blur = false
    }
}
```

#### Minimalist Setup
```conf
plugin {
    carousel {
        spacing = 1.0
        transparency_gradient = 0.1
        show_workspace_names = false
        background_blur = false
    }
}
```

## Configuration Validation

### Check Configuration Syntax
After modifying your configuration, check for syntax errors:
```bash
hyprctl reload
```

If there are errors, Hyprland will log them to:
```bash
journalctl -u hyprland.service --since "1 minute ago"
```

### Test Configuration Changes
Use dynamic configuration to test settings without restarting:
```bash
# Test animation duration change
hyprctl keyword plugin:carousel:animation_duration 500

# Test radius change  
hyprctl keyword plugin:carousel:radius 1000
```

### Reset to Defaults
If configuration issues occur, temporarily disable the plugin:
```conf
# plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so
```

Then restart Hyprland and reconfigure step by step.

## Configuration Examples

### Example 1: Gaming Setup
Optimized for performance during gaming:
```conf
plugin {
    carousel {
        radius = 600
        animation_duration = 150
        texture_quality = medium
        max_workspaces = 5
        vsync = false
        background_blur = false
    }
}
```

### Example 2: Productivity Setup
Optimized for many workspaces and visual clarity:
```conf
plugin {
    carousel {
        radius = 1000
        spacing = 1.5
        animation_duration = 400
        max_workspaces = 12
        show_workspace_names = true
        transparency_gradient = 0.4
    }
}
```

### Example 3: Presentation Setup
Optimized for visual appeal and smooth demonstrations:
```conf
plugin {
    carousel {
        radius = 1200
        spacing = 2.0
        animation_duration = 600
        selected_scale = 1.3
        transparency_gradient = 0.6
        texture_quality = ultra
    }
}
```

## Troubleshooting Configuration

### Common Issues

#### Plugin Not Loading
- Check file path in configuration
- Verify plugin file exists and is executable
- Check Hyprland logs for error messages

#### Configuration Options Not Applied
- Verify syntax in plugin block
- Check option spelling and case sensitivity
- Ensure values are within valid ranges

#### Performance Issues
- Reduce `texture_quality`
- Lower `max_workspaces`
- Disable `background_blur`
- Reduce `radius` and animation complexity

#### Visual Glitches
- Check graphics driver compatibility
- Verify OpenGL 3.3+ support
- Try different `texture_quality` settings

### Debug Configuration
Enable debug logging (if available):
```conf
plugin {
    carousel {
        debug_mode = true
        log_level = verbose
    }
}
```

For detailed troubleshooting, see the [Troubleshooting Guide](troubleshooting.md).

## Next Steps

After configuring the plugin:
1. **Learn usage**: See [Usage Guide](usage.md)
2. **Customize keybindings**: See [Keybindings Reference](keybindings.md)  
3. **Optimize performance**: See [Performance Guide](performance.md)