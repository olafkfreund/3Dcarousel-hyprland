# API Reference

This document provides complete API reference for the Hyprland 3D Carousel Plugin.

**WARNING: This plugin is in early development stages and is not ready for production use.**

## Plugin API

### Plugin Entry Points

The plugin implements the standard Hyprland plugin interface:

#### `pluginAPIVersion()`
```cpp
APICALL EXPORT const char* pluginAPIVersion()
```
- **Returns**: Hyprland API version string
- **Description**: Returns the API version the plugin was built for
- **Usage**: Called by Hyprland during plugin loading to verify compatibility

#### `pluginInit()`
```cpp
APICALL EXPORT PLUGIN_DESCRIPTION_INFO pluginInit(HANDLE handle)
```
- **Parameters**: 
  - `handle`: Plugin handle provided by Hyprland
- **Returns**: Plugin description information structure
- **Description**: Initializes the plugin and registers dispatchers
- **Usage**: Called once when plugin is loaded

#### `pluginExit()`
```cpp
APICALL EXPORT void pluginExit()
```
- **Parameters**: None
- **Returns**: void
- **Description**: Cleanup function called when plugin is unloaded
- **Usage**: Called once when plugin is being removed

## Dispatcher Commands

### Core Carousel Commands

#### `carousel:toggle`
```bash
hyprctl dispatch carousel:toggle
```
- **Description**: Toggles carousel mode on/off
- **Behavior**: 
  - If carousel is inactive: Captures workspaces and enters carousel mode
  - If carousel is active: Exits carousel mode and returns to original workspace
- **Requirements**: At least one workspace with windows

#### `carousel:next`
```bash
hyprctl dispatch carousel:next
```
- **Description**: Moves to the next workspace in the carousel
- **Behavior**: Rotates carousel to highlight the next workspace
- **Requirements**: Carousel must be active
- **Wrapping**: Wraps around to first workspace after the last

#### `carousel:prev`
```bash
hyprctl dispatch carousel:prev
```
- **Description**: Moves to the previous workspace in the carousel
- **Behavior**: Rotates carousel to highlight the previous workspace
- **Requirements**: Carousel must be active
- **Wrapping**: Wraps around to last workspace before the first

#### `carousel:select`
```bash
hyprctl dispatch carousel:select
```
- **Description**: Switches to the currently highlighted workspace
- **Behavior**: 
  - Plays selection animation
  - Switches to the highlighted workspace
  - Exits carousel mode
- **Requirements**: Carousel must be active

#### `carousel:exit`
```bash
hyprctl dispatch carousel:exit
```
- **Description**: Exits carousel mode without switching workspaces
- **Behavior**: 
  - Returns to the workspace that was active before carousel was triggered
  - Exits carousel mode
  - No workspace switching occurs
- **Requirements**: Carousel must be active

## Plugin Classes

### CarouselPlugin

Main plugin singleton class that manages the entire plugin lifecycle.

#### Methods

##### `getInstance()`
```cpp
static CarouselPlugin& getInstance()
```
- **Returns**: Reference to the singleton instance
- **Description**: Gets or creates the plugin singleton instance

##### `initialize()`
```cpp
bool initialize()
```
- **Returns**: `true` if initialization successful, `false` otherwise
- **Description**: Initializes all plugin components (renderer, workspace manager, animation engine)
- **Side Effects**: Creates and initializes component objects

##### `cleanup()`
```cpp
void cleanup()
```
- **Returns**: void
- **Description**: Cleans up all plugin resources
- **Side Effects**: Destroys component objects and frees resources

##### `toggleCarousel()`
```cpp
void toggleCarousel()
```
- **Returns**: void
- **Description**: Implementation of carousel:toggle dispatcher
- **Side Effects**: Enters/exits carousel mode

##### `nextWorkspace()`
```cpp
void nextWorkspace()
```
- **Returns**: void
- **Description**: Implementation of carousel:next dispatcher
- **Side Effects**: Updates workspace selection and triggers animation

##### `prevWorkspace()`
```cpp
void prevWorkspace()
```
- **Returns**: void
- **Description**: Implementation of carousel:prev dispatcher
- **Side Effects**: Updates workspace selection and triggers animation

##### `selectWorkspace()`
```cpp
void selectWorkspace()
```
- **Returns**: void
- **Description**: Implementation of carousel:select dispatcher
- **Side Effects**: Switches workspace and exits carousel

##### `exitCarousel()`
```cpp
void exitCarousel()
```
- **Returns**: void
- **Description**: Implementation of carousel:exit dispatcher
- **Side Effects**: Exits carousel mode

### CarouselRenderer

Handles 3D rendering and OpenGL operations.

#### Methods

##### `initialize()`
```cpp
bool initialize()
```
- **Returns**: `true` if renderer initialization successful
- **Description**: Creates shaders, vertex buffers, and OpenGL resources

##### `cleanup()`
```cpp
void cleanup()
```
- **Returns**: void
- **Description**: Releases all OpenGL resources

##### `render()`
```cpp
void render()
```
- **Returns**: void
- **Description**: Renders the current carousel frame
- **Side Effects**: Draws to the current OpenGL context

##### `updateWorkspacePositions()`
```cpp
void updateWorkspacePositions(float rotation, int selectedIndex)
```
- **Parameters**:
  - `rotation`: Current carousel rotation angle in radians
  - `selectedIndex`: Index of currently selected workspace
- **Returns**: void
- **Description**: Updates 3D positions and properties of all workspace frames

##### `captureWorkspaces()`
```cpp
void captureWorkspaces()
```
- **Returns**: void
- **Description**: Captures current workspace contents as textures
- **Side Effects**: Creates texture objects for each workspace

##### Configuration Methods

```cpp
void setCarouselRadius(float radius)
void setWorkspaceSpacing(float spacing)
void setTransparencyGradient(float gradient)
```
- **Parameters**: Configuration value within valid range
- **Returns**: void
- **Description**: Updates rendering parameters

### WorkspaceManager

Manages workspace selection and navigation logic.

#### Methods

##### `initialize()`
```cpp
bool initialize()
```
- **Returns**: `true` if initialization successful
- **Description**: Initializes workspace tracking

##### `selectNext()`
```cpp
void selectNext()
```
- **Returns**: void
- **Description**: Moves selection to next workspace with wrapping

##### `selectPrevious()`
```cpp
void selectPrevious()
```
- **Returns**: void
- **Description**: Moves selection to previous workspace with wrapping

##### `selectWorkspace()`
```cpp
void selectWorkspace(int index)
```
- **Parameters**: `index` - Workspace index to select
- **Returns**: void
- **Description**: Directly selects a specific workspace

##### `getCurrentSelection()`
```cpp
int getCurrentSelection() const
```
- **Returns**: Index of currently selected workspace
- **Description**: Gets the current workspace selection

##### `getWorkspaceCount()`
```cpp
int getWorkspaceCount() const
```
- **Returns**: Number of available workspaces
- **Description**: Gets total count of workspaces in carousel

##### `refreshWorkspaces()`
```cpp
void refreshWorkspaces()
```
- **Returns**: void
- **Description**: Updates workspace list from Hyprland
- **Side Effects**: May change workspace count and selection

### AnimationEngine

Handles smooth animations and transitions.

#### Methods

##### `initialize()`
```cpp
bool initialize()
```
- **Returns**: `true` if initialization successful
- **Description**: Initializes animation system

##### `startCarouselAnimation()`
```cpp
void startCarouselAnimation()
```
- **Returns**: void
- **Description**: Begins carousel rotation animation

##### `stopCarouselAnimation()`
```cpp
void stopCarouselAnimation()
```
- **Returns**: void
- **Description**: Stops current animation

##### `startGrowAnimation()`
```cpp
void startGrowAnimation()
```
- **Returns**: void
- **Description**: Starts workspace selection grow animation

##### `update()`
```cpp
void update()
```
- **Returns**: void
- **Description**: Updates animation state (called per frame)

##### `getCurrentRotation()`
```cpp
float getCurrentRotation() const
```
- **Returns**: Current rotation angle in radians
- **Description**: Gets current carousel rotation for rendering

##### `getGrowScale()`
```cpp
float getGrowScale() const
```
- **Returns**: Current grow scale factor
- **Description**: Gets current selection animation scale

##### `isAnimating()`
```cpp
bool isAnimating() const
```
- **Returns**: `true` if animation is in progress
- **Description**: Checks if any animation is currently running

##### `setAnimationDuration()`
```cpp
void setAnimationDuration(int durationMs)
```
- **Parameters**: `durationMs` - Animation duration in milliseconds
- **Returns**: void
- **Description**: Sets animation duration for future animations

## Data Structures

### WorkspaceFrame

Represents a single workspace in the carousel.

```cpp
struct WorkspaceFrame {
    GLuint texture;        // OpenGL texture ID
    GLuint framebuffer;    // OpenGL framebuffer ID  
    int width;             // Texture width in pixels
    int height;            // Texture height in pixels
    float angle;           // Current angle in carousel (radians)
    float distance;        // Distance from carousel center
    float alpha;           // Transparency value (0.0-1.0)
}
```

## Configuration Integration

### Configuration Keys

The plugin reads configuration from Hyprland's plugin configuration system:

```cpp
// Example configuration access
float radius = configManager->getFloat("plugin:carousel:radius", 800.0f);
float spacing = configManager->getFloat("plugin:carousel:spacing", 1.2f);
int duration = configManager->getInt("plugin:carousel:animation_duration", 300);
bool blur = configManager->getBool("plugin:carousel:background_blur", true);
```

### Dynamic Configuration

Configuration changes can be applied at runtime:
```bash
hyprctl keyword plugin:carousel:radius 1000
hyprctl keyword plugin:carousel:animation_duration 500
```

## Error Handling

### Version Checking

The plugin implements strict version checking:
```cpp
const std::string HASH = __hyprland_api_get_hash();
if (HASH != GIT_COMMIT_HASH) {
    throw std::runtime_error("API version mismatch");
}
```

### Initialization Errors

All initialization methods return boolean success indicators:
```cpp
if (!renderer->initialize()) {
    HyprlandAPI::addNotification(PHANDLE, "Failed to initialize renderer", 
                               CColor{1.0, 0.0, 0.0, 1.0}, 5000);
    return false;
}
```

### Runtime Error Handling

The plugin includes defensive programming practices:
- Null pointer checks before component usage
- Boundary validation for workspace indices
- OpenGL error checking after graphics operations
- Graceful degradation for missing features

## Performance Considerations

### Memory Management

- **RAII Patterns**: All OpenGL resources managed with RAII
- **Smart Pointers**: Components managed with `std::unique_ptr`
- **Texture Cleanup**: Automatic cleanup when exiting carousel

### Optimization Features

- **Texture Caching**: Workspaces captured only when needed
- **LOD System**: Lower quality for distant workspaces
- **Frustum Culling**: Hidden workspaces not rendered
- **VSync Control**: Configurable vertical synchronization

## Integration Points

### Hyprland Integration

The plugin integrates with Hyprland through:
- **Plugin API**: Standard Hyprland plugin interface
- **Dispatcher System**: Custom command registration
- **Configuration System**: Plugin configuration blocks
- **Notification System**: User feedback and error reporting
- **Workspace System**: Live workspace state access

### OpenGL Integration

- **Context Sharing**: Uses Hyprland's OpenGL context
- **Resource Management**: Proper OpenGL resource lifecycle
- **Shader System**: Custom vertex and fragment shaders
- **Texture Management**: Workspace capture and rendering

## Future API Extensions

Planned API extensions for future versions:

### Additional Dispatchers
- `carousel:goto <workspace>` - Direct workspace navigation
- `carousel:config <setting> <value>` - Runtime configuration
- `carousel:info` - Display current carousel state

### Enhanced Configuration
- Per-monitor carousel settings
- Custom animation curves
- Workspace filtering options

### Callback System
- Workspace switch callbacks
- Animation completion events
- Configuration change notifications

This API reference covers the current implementation. Future versions may extend or modify these interfaces.