# Plugin Debug Information

## Updated Plugin Path
```
/nix/store/nfc53nich423fm5yd6wcq3j30kmyq22q-hypr-carousel-1.0.0/lib/hypr-carousel.so
```

## What was fixed:
1. **Better error handling** in initialization
2. **Default configuration** to avoid config loading failures
3. **Detailed notifications** to show what's happening
4. **Try-catch blocks** around config registration

## Expected notifications when loading:
1. `[Carousel] Initialized with defaults!` (green) - initialization success
2. `[Carousel] Plugin loaded successfully!` (green) - full loading success

## If initialization still fails:
Look for these notifications to debug:
- `[Carousel] g_pCompositor is null!` (red) - compositor not available
- `[Carousel] Config registration failed, using defaults` (yellow) - config issues
- `[Carousel] Failed to initialize!` (red) - general initialization failure

## Minimal test configuration:
Add to hyprland.conf:
```conf
plugin = /nix/store/nfc53nich423fm5yd6wcq3j30kmyq22q-hypr-carousel-1.0.0/lib/hypr-carousel.so
bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
```

Then restart Hyprland and watch for notifications.

## Manual dispatch test:
```bash
hyprctl dispatch carousel:toggle
```

This should work if the plugin loaded successfully.