# Usage Guide

This guide covers how to use the Hyprland 3D Carousel Plugin for workspace navigation.

**WARNING: This plugin is in early development stages and is not ready for production use.**

## Basic Usage

### Activating the Carousel

To enter carousel mode, use the toggle command:
```bash
hyprctl dispatch carousel:toggle
```

Default keybinding: `SUPER + TAB`

When activated, the carousel will:
1. Capture all active workspaces
2. Display them in a 3D circular arrangement
3. Highlight the currently selected workspace
4. Allow navigation between workspaces

### Navigating Workspaces

Once in carousel mode:

#### Move to Next Workspace
```bash
hyprctl dispatch carousel:next
```
Default keybinding: `SUPER + Right Arrow`

#### Move to Previous Workspace  
```bash
hyprctl dispatch carousel:prev
```
Default keybinding: `SUPER + Left Arrow`

#### Select Current Workspace
```bash
hyprctl dispatch carousel:select
```
Default keybinding: `SUPER + Return`

This will:
1. Switch to the highlighted workspace
2. Exit carousel mode
3. Show the workspace transition animation

#### Exit Without Selection
```bash
hyprctl dispatch carousel:exit
```
Default keybinding: `SUPER + Escape`

This exits carousel mode without changing workspaces.

## Workflow Examples

### Example 1: Quick Workspace Switching
```
1. Press SUPER + TAB (activate carousel)
2. Press SUPER + Right Arrow (navigate to desired workspace)
3. Press SUPER + Return (switch to workspace)
```

### Example 2: Workspace Overview
```
1. Press SUPER + TAB (activate carousel)
2. Use SUPER + Left/Right to browse all workspaces
3. Press SUPER + Escape (exit without switching)
```

### Example 3: Rapid Navigation
```
1. Press SUPER + TAB (activate carousel)
2. Repeatedly press SUPER + Right to cycle through workspaces
3. Press SUPER + Return when desired workspace is highlighted
```

## Advanced Usage

### Understanding Workspace Selection

The carousel displays workspaces with different visual states:

#### Selected Workspace
- **Position**: Center-front of the carousel
- **Scale**: Larger than other workspaces
- **Transparency**: Fully opaque
- **Highlighting**: May have additional visual effects

#### Adjacent Workspaces
- **Position**: Slightly to left/right of center
- **Scale**: Normal size
- **Transparency**: Slightly transparent

#### Distant Workspaces
- **Position**: Far left/right in the carousel
- **Scale**: Smaller than normal
- **Transparency**: More transparent, fading with distance

### Navigation Patterns

#### Linear Navigation
Navigate workspaces in order:
```
Workspace 1 → 2 → 3 → 4 → 5 → 1 (wraps around)
```

#### Skip Navigation
Quickly move through multiple workspaces:
```
Hold SUPER + Right Arrow to rapidly cycle through workspaces
```

#### Return to Original
The carousel remembers your starting workspace:
```
1. Start on Workspace 3
2. Navigate to Workspace 7 in carousel
3. Press SUPER + Escape
4. Returns to Workspace 3
```

## Visual Features

### 3D Carousel Effect
- Workspaces arranged in a circular 3D layout
- Smooth rotation animations between positions
- Depth-based perspective with closer workspaces appearing larger

### Transparency Gradient
- Non-selected workspaces fade based on distance
- Creates focus on the currently selected workspace
- Configurable fade intensity

### Workspace Previews
- Live capture of workspace contents
- Real-time preview of applications and windows
- Scaled representation of actual workspace layout

### Animation Effects

#### Rotation Animation
- Smooth interpolation between workspace positions
- Configurable animation duration
- Easing functions for natural movement

#### Selection Animation
- Visual feedback when highlighting different workspaces
- Scale and opacity changes
- Smooth transitions between states

#### Entry/Exit Animation
- Fade-in effect when activating carousel
- Fade-out effect when exiting
- Workspace grow animation when selecting

## Integration with Hyprland

### Workspace State
The carousel integrates with Hyprland's workspace system:
- Displays only workspaces with active windows
- Respects Hyprland's workspace numbering
- Updates dynamically as workspaces are created/destroyed

### Window Management
While in carousel mode:
- Windows continue running normally
- No interruption to running applications
- Workspace contents remain interactive

### Multi-Monitor Support
On multi-monitor setups:
- Carousel displays on the currently focused monitor
- Shows workspaces relevant to the current monitor
- Respects per-monitor workspace configuration

## Performance Considerations

### Texture Capture
- Workspaces are captured as textures when carousel activates
- Higher quality settings use more memory
- Capture frequency affects performance

### Rendering Performance
- 3D rendering uses GPU acceleration
- Performance scales with number of workspaces
- Animation complexity affects frame rate

### Memory Usage
- Texture storage increases with workspace count
- Higher quality settings use more VRAM
- Automatic cleanup when exiting carousel

## Tips and Best Practices

### Efficient Navigation
1. **Learn the pattern**: Remember that workspaces wrap around
2. **Use previews**: Look at workspace contents before selecting
3. **Quick exit**: Use Escape if you activate carousel accidentally

### Performance Optimization
1. **Limit workspaces**: Keep active workspace count reasonable
2. **Adjust quality**: Lower texture quality on slower systems
3. **Close unused workspaces**: Remove empty workspaces periodically

### Workflow Integration
1. **Consistent usage**: Use carousel for all workspace switching
2. **Combine with other tools**: Works well with workspace management scripts
3. **Customize keybindings**: Adapt bindings to your workflow

## Common Use Cases

### Development Workflow
```
Workspace 1: IDE and terminal
Workspace 2: Web browser for research
Workspace 3: Documentation and notes
Workspace 4: Communication (Slack, email)
Workspace 5: Testing environment
```

Navigation: Use carousel to quickly preview and switch between development contexts.

### Content Creation
```
Workspace 1: Video editor
Workspace 2: Image editor
Workspace 3: File browser and assets
Workspace 4: Preview and playback
Workspace 5: Notes and scripts
```

Navigation: Visual preview helps identify which workspace contains which tools.

### System Administration
```
Workspace 1: Main terminal sessions
Workspace 2: Monitoring dashboards
Workspace 3: Documentation and references
Workspace 4: Communication and tickets
Workspace 5: Testing and development
```

Navigation: Quickly switch between operational contexts while maintaining overview.

## Limitations

### Current Limitations
- Requires OpenGL 3.3+ support
- Performance depends on GPU capabilities
- Limited to workspaces with active windows
- May not work correctly with some window managers

### Known Issues
- Texture capture may not work with all application types
- Performance can degrade with many workspaces
- Some visual effects may not work on all graphics drivers

For troubleshooting common issues, see the [Troubleshooting Guide](troubleshooting.md).

## Next Steps

To enhance your carousel experience:
1. **Customize configuration**: See [Configuration Guide](configuration.md)
2. **Set up custom keybindings**: See [Keybindings Reference](keybindings.md)
3. **Optimize performance**: See [Performance Guide](performance.md)
4. **Report issues**: See [Troubleshooting Guide](troubleshooting.md)