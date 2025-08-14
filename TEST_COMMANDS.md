# 3D Carousel Plugin Test Commands

## Plugin Location
```
/nix/store/8wdg1md6f4xs94namvmy0srksi9gdaa5-hypr-carousel-1.0.0/lib/hypr-carousel.so
```

## Step 1: Enable Plugin
Add this line to your `~/.config/hypr/hyprland.conf`:
```conf
plugin = /nix/store/8wdg1md6f4xs94namvmy0srksi9gdaa5-hypr-carousel-1.0.0/lib/hypr-carousel.so
```

## Step 2: Add Keybindings
Add these keybindings to test all dispatch commands:
```conf
bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
bind = SUPER, right, exec, hyprctl dispatch carousel:next  
bind = SUPER, left, exec, hyprctl dispatch carousel:prev
bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select
bind = SUPER, ESCAPE, exec, hyprctl dispatch carousel:exit
```

## Step 3: Restart Hyprland
Restart Hyprland to load the plugin.

## Step 4: Test All Commands

### Test 1: Toggle Carousel (SUPER + TAB)
- Should activate 3D carousel view
- Expect notification: "Carousel activated"

### Test 2: Navigate Right (SUPER + Right Arrow)  
- Should rotate carousel to next workspace
- Workspaces should move in 3D circle

### Test 3: Navigate Left (SUPER + Left Arrow)
- Should rotate carousel to previous workspace  
- Should rotate in opposite direction

### Test 4: Select Workspace (SUPER + RETURN)
- Should switch to highlighted workspace
- Should exit carousel mode
- Should switch to the selected workspace

### Test 5: Exit Carousel (SUPER + ESCAPE)
- Should exit carousel without switching workspaces
- Should return to normal workspace view
- Expect notification: "Carousel deactivated"

## Manual Testing Commands
You can also test manually with:
```bash
hyprctl dispatch carousel:toggle
hyprctl dispatch carousel:next
hyprctl dispatch carousel:prev  
hyprctl dispatch carousel:select
hyprctl dispatch carousel:exit
```

## Expected Plugin Behavior
- **3D View**: Workspaces arranged in rotating circle
- **Perspective**: Distant workspaces smaller and more transparent
- **Selection**: Current workspace highlighted with full opacity
- **Smooth Rotation**: Fluid animation between workspaces
- **Safe Operation**: No crashes, no window position changes

## Configuration (Optional)
The plugin uses these defaults (can be customized):
- radius = 800 pixels
- spacing = 1.2
- transparency_gradient = 0.3  
- animation_duration = 300ms