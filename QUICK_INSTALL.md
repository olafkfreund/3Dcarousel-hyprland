# Quick Installation Guide

## 🚀 Your 3D Carousel Plugin is Ready!

The plugin has been successfully built and is ready for installation.

### ✅ Plugin Status
- **Built**: ✅ Nix build completed successfully
- **Size**: 107KB optimized binary
- **Location**: `~/.local/share/hyprland/plugins/hypr-carousel.so`
- **Version**: Compatible with Hyprland 0.50.1+

### 📦 Installation Steps

1. **Plugin is already installed** at `~/.local/share/hyprland/plugins/hypr-carousel.so`

2. **Add to your Hyprland configuration**:
   ```bash
   # Edit your hyprland config
   $EDITOR ~/.config/hypr/hyprland.conf
   
   # Add this line to load the plugin
   plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so
   ```

3. **Add the configuration from `hyprland-config-snippet.conf`**:
   ```conf
   # Plugin settings
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

4. **Restart Hyprland**:
   ```bash
   # Method 1: Reload config (if plugin loading is supported)
   hyprctl reload
   
   # Method 2: Full restart (recommended for plugins)
   # Log out and log back in, or restart your session
   ```

### 🎮 Usage

1. **Activate carousel**: Press `SUPER + TAB`
2. **Navigate**: Use `SUPER + ←/→` to rotate through workspaces
3. **Select**: Press `SUPER + RETURN` to switch to highlighted workspace
4. **Exit**: Press `SUPER + ESCAPE` to cancel without switching

### 🔧 Verification

After restarting Hyprland, you should see:
- A notification: "[Carousel] Plugin loaded successfully!"
- Carousel commands available: `hyprctl dispatch carousel:toggle`

### 🐛 Troubleshooting

If the plugin doesn't load:

1. **Check plugin exists**:
   ```bash
   ls -la ~/.local/share/hyprland/plugins/hypr-carousel.so
   ```

2. **Check Hyprland logs**:
   ```bash
   journalctl -u user@$(id -u).service --since "1 hour ago" | grep -i hypr
   ```

3. **Verify plugin symbols**:
   ```bash
   nm ~/.local/share/hyprland/plugins/hypr-carousel.so | grep plugin
   ```

4. **Test minimal loading**:
   ```bash
   # Remove plugin from config, restart, then try manual load
   hyprctl plugin load ~/.local/share/hyprland/plugins/hypr-carousel.so
   ```

### 🎯 Expected Behavior

When working correctly:
- **3D View**: Workspaces arranged in a rotating circle
- **Depth Effects**: Distant workspaces appear smaller and more transparent
- **Smooth Animation**: Fluid rotation as you navigate
- **Visual Feedback**: Selected workspace highlighted with full opacity

### 📋 Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `radius` | 800 | Carousel radius in pixels |
| `spacing` | 1.2 | Spacing between workspaces |
| `transparency_gradient` | 0.3 | Depth transparency effect |
| `animation_duration` | 300 | Animation speed in ms |

### 🚀 Ready to Use!

Your 3D carousel plugin is now ready for an immersive workspace navigation experience in Hyprland!

---

*Built with Claude Code - Modern C++23 implementation with full Hyprland 0.50.1 compatibility*