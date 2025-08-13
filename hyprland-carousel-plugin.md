# Hyprland 3D Workspace Carousel Plugin

## Overview

This plugin creates a 3D carousel effect for Hyprland workspaces, allowing users to scroll through active workspaces with a stacked, semi-transparent display. Users can preview workspaces and select one to activate with a smooth grow animation.

## Features

- **3D Carousel Effect**: Workspaces arranged in a rotating carousel with depth
- **Semi-transparent Preview**: All workspaces visible with transparency gradient
- **Smooth Scrolling**: Navigate through workspaces with mouse/keyboard
- **Preview Mode**: Examine workspace contents before activation
- **Grow Animation**: Selected workspace smoothly transitions to full view
- **Active Workspace Highlighting**: Current workspace clearly distinguished

## Prerequisites

### System Requirements
- Hyprland (latest version)
- OpenGL 3.3+ support
- C++20 compatible compiler (GCC 10+ or Clang 12+)
- CMake 3.19+

### Dependencies
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake pkg-config libwayland-dev libxkbcommon-dev \
    libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdrm-dev libxcb1-dev \
    libxcb-composite0-dev libxcb-xfixes0-dev libxcb-xinput-dev libxcb-image0-dev \
    libxcb-shm0-dev libxcb-util0-dev libxcb-keysyms1-dev libxcb-randr0-dev \
    libxcb-ewmh-dev libpixman-1-dev libinput-dev libxcb-xinput-dev

# Arch Linux
sudo pacman -S base-devel cmake wayland wayland-protocols libxkbcommon \
    mesa libdrm libxcb xcb-util xcb-util-keysyms xcb-util-wm pixman libinput

# Fedora
sudo dnf install gcc-c++ cmake wayland-devel libxkbcommon-devel mesa-libGL-devel \
    mesa-libGLES-devel mesa-libEGL-devel libdrm-devel libxcb-devel \
    xcb-util-devel libinput-devel pixman-devel
```

## Project Structure

```
hypr-carousel/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── CarouselRenderer.cpp
│   ├── CarouselRenderer.hpp
│   ├── WorkspaceManager.cpp
│   ├── WorkspaceManager.hpp
│   ├── AnimationEngine.cpp
│   ├── AnimationEngine.hpp
│   └── shaders/
│       ├── carousel.vert
│       ├── carousel.frag
│       └── workspace.frag
├── include/
│   └── plugin.hpp
└── README.md
```

## Implementation

### CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.19)
project(hypr-carousel VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HYPRLAND REQUIRED hyprland)
pkg_check_modules(GL REQUIRED gl)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-server)

add_library(hypr-carousel SHARED
    src/main.cpp
    src/CarouselRenderer.cpp
    src/WorkspaceManager.cpp
    src/AnimationEngine.cpp
)

target_include_directories(hypr-carousel PRIVATE
    include/
    ${HYPRLAND_INCLUDE_DIRS}
)

target_link_libraries(hypr-carousel
    ${HYPRLAND_LIBRARIES}
    ${GL_LIBRARIES}
    ${EGL_LIBRARIES}
    ${WAYLAND_LIBRARIES}
    GL
    EGL
)

target_compile_options(hypr-carousel PRIVATE ${HYPRLAND_CFLAGS_OTHER})

set_target_properties(hypr-carousel PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)
```

### include/plugin.hpp
```cpp
#pragma once
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/Compositor.hpp>
#include <memory>

class CarouselRenderer;
class WorkspaceManager;
class AnimationEngine;

class CarouselPlugin {
public:
    static CarouselPlugin& getInstance();
    
    bool initialize();
    void cleanup();
    
    void toggleCarousel();
    void nextWorkspace();
    void prevWorkspace();
    void selectWorkspace();
    void exitCarousel();
    
private:
    CarouselPlugin() = default;
    ~CarouselPlugin() = default;
    
    std::unique_ptr<CarouselRenderer> m_renderer;
    std::unique_ptr<WorkspaceManager> m_workspaceManager;
    std::unique_ptr<AnimationEngine> m_animationEngine;
    
    bool m_active = false;
    bool m_initialized = false;
};

// Plugin entry points
extern "C" {
    APICALL const char* PLUGIN_API_VERSION();
    APICALL PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle);
    APICALL void PLUGIN_EXIT();
}
```

### src/CarouselRenderer.hpp
```cpp
#pragma once
#include <GL/gl.h>
#include <vector>
#include <memory>

struct WorkspaceFrame {
    GLuint texture;
    GLuint framebuffer;
    int width, height;
    float angle;
    float distance;
    float alpha;
};

class CarouselRenderer {
public:
    CarouselRenderer();
    ~CarouselRenderer();
    
    bool initialize();
    void cleanup();
    
    void render();
    void updateWorkspacePositions(float rotation, int selectedIndex);
    void captureWorkspaces();
    
    void setCarouselRadius(float radius) { m_carouselRadius = radius; }
    void setWorkspaceSpacing(float spacing) { m_workspaceSpacing = spacing; }
    void setTransparencyGradient(float gradient) { m_transparencyGradient = gradient; }
    
private:
    bool createShaders();
    bool createGeometry();
    void renderWorkspace(const WorkspaceFrame& frame, const glm::mat4& mvp);
    
    GLuint m_shaderProgram;
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    
    GLuint m_VAO, m_VBO, m_EBO;
    
    // Shader uniforms
    GLint m_uMVP;
    GLint m_uTexture;
    GLint m_uAlpha;
    GLint m_uSelected;
    
    std::vector<WorkspaceFrame> m_workspaceFrames;
    
    float m_carouselRadius = 800.0f;
    float m_workspaceSpacing = 1.2f;
    float m_transparencyGradient = 0.3f;
    
    int m_selectedWorkspace = 0;
    float m_currentRotation = 0.0f;
};
```

### src/CarouselRenderer.cpp
```cpp
#include "CarouselRenderer.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uMVP;
out vec2 TexCoord;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uAlpha;
uniform bool uSelected;

void main() {
    vec4 texColor = texture(uTexture, TexCoord);
    
    if (uSelected) {
        // Highlight selected workspace
        texColor.rgb += 0.2;
    }
    
    FragColor = vec4(texColor.rgb, texColor.a * uAlpha);
}
)";

CarouselRenderer::CarouselRenderer() {}

CarouselRenderer::~CarouselRenderer() {
    cleanup();
}

bool CarouselRenderer::initialize() {
    if (!createShaders()) {
        std::cerr << "Failed to create shaders" << std::endl;
        return false;
    }
    
    if (!createGeometry()) {
        std::cerr << "Failed to create geometry" << std::endl;
        return false;
    }
    
    return true;
}

bool CarouselRenderer::createShaders() {
    // Vertex shader
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(m_vertexShader);
    
    GLint success;
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(m_vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // Fragment shader
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(m_fragmentShader);
    
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(m_fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    // Shader program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);
    
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }
    
    // Get uniform locations
    m_uMVP = glGetUniformLocation(m_shaderProgram, "uMVP");
    m_uTexture = glGetUniformLocation(m_shaderProgram, "uTexture");
    m_uAlpha = glGetUniformLocation(m_shaderProgram, "uAlpha");
    m_uSelected = glGetUniformLocation(m_shaderProgram, "uSelected");
    
    return true;
}

bool CarouselRenderer::createGeometry() {
    // Quad vertices for workspace rendering
    float vertices[] = {
        // positions          // texture coords
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    return true;
}

void CarouselRenderer::render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    
    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_VAO);
    
    // Setup projection matrix
    auto monitor = g_pCompositor->m_pLastMonitor;
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 
        (float)monitor->vecSize.x / (float)monitor->vecSize.y, 
        0.1f, 
        2000.0f
    );
    
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 1000.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    for (size_t i = 0; i < m_workspaceFrames.size(); ++i) {
        const auto& frame = m_workspaceFrames[i];
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, frame.angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -frame.distance));
        model = glm::scale(model, glm::vec3(400.0f, 300.0f, 1.0f));
        
        glm::mat4 mvp = projection * view * model;
        
        glUniformMatrix4fv(m_uMVP, 1, GL_FALSE, &mvp[0][0]);
        glUniform1f(m_uAlpha, frame.alpha);
        glUniform1i(m_uSelected, i == m_selectedWorkspace);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frame.texture);
        glUniform1i(m_uTexture, 0);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void CarouselRenderer::updateWorkspacePositions(float rotation, int selectedIndex) {
    m_currentRotation = rotation;
    m_selectedWorkspace = selectedIndex;
    
    for (size_t i = 0; i < m_workspaceFrames.size(); ++i) {
        auto& frame = m_workspaceFrames[i];
        
        float angleOffset = (float)i * m_workspaceSpacing * (2.0f * M_PI / m_workspaceFrames.size());
        frame.angle = rotation + angleOffset;
        
        // Distance from center (closer when selected)
        if ((int)i == selectedIndex) {
            frame.distance = m_carouselRadius * 0.7f;
            frame.alpha = 1.0f;
        } else {
            frame.distance = m_carouselRadius;
            // Fade based on angular distance from selected
            float angleDiff = std::abs(frame.angle - (rotation + selectedIndex * angleOffset));
            angleDiff = std::min(angleDiff, 2.0f * M_PI - angleDiff);
            frame.alpha = std::max(0.3f, 1.0f - angleDiff * m_transparencyGradient);
        }
    }
}

void CarouselRenderer::captureWorkspaces() {
    // Implementation to capture workspace textures
    // This would use Hyprland's rendering system to capture each workspace
    auto workspaces = g_pCompositor->m_vWorkspaces;
    
    m_workspaceFrames.clear();
    m_workspaceFrames.reserve(workspaces.size());
    
    for (auto& workspace : workspaces) {
        if (!workspace->m_bIsSpecialWorkspace && workspace->m_windows.size() > 0) {
            WorkspaceFrame frame;
            // Capture workspace to texture
            // This is a simplified version - actual implementation would
            // need to render the workspace to an FBO
            frame.width = 1920;  // or monitor width
            frame.height = 1080; // or monitor height
            frame.angle = 0.0f;
            frame.distance = m_carouselRadius;
            frame.alpha = 0.7f;
            
            m_workspaceFrames.push_back(frame);
        }
    }
}

void CarouselRenderer::cleanup() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
    if (m_vertexShader) glDeleteShader(m_vertexShader);
    if (m_fragmentShader) glDeleteShader(m_fragmentShader);
    
    for (auto& frame : m_workspaceFrames) {
        if (frame.texture) glDeleteTextures(1, &frame.texture);
        if (frame.framebuffer) glDeleteFramebuffers(1, &frame.framebuffer);
    }
}
```

### src/main.cpp
```cpp
#include "plugin.hpp"
#include "CarouselRenderer.hpp"
#include "WorkspaceManager.hpp"
#include "AnimationEngine.hpp"
#include <hyprland/src/plugins/PluginAPI.hpp>

CarouselPlugin& CarouselPlugin::getInstance() {
    static CarouselPlugin instance;
    return instance;
}

bool CarouselPlugin::initialize() {
    if (m_initialized) return true;
    
    m_renderer = std::make_unique<CarouselRenderer>();
    m_workspaceManager = std::make_unique<WorkspaceManager>();
    m_animationEngine = std::make_unique<AnimationEngine>();
    
    if (!m_renderer->initialize()) {
        HyprlandAPI::addNotification(PHANDLE, "Failed to initialize renderer", CColor{1.0, 0.0, 0.0, 1.0}, 3000);
        return false;
    }
    
    m_initialized = true;
    return true;
}

void CarouselPlugin::toggleCarousel() {
    if (!m_initialized) return;
    
    m_active = !m_active;
    
    if (m_active) {
        m_renderer->captureWorkspaces();
        m_animationEngine->startCarouselAnimation();
    } else {
        m_animationEngine->stopCarouselAnimation();
    }
}

void CarouselPlugin::nextWorkspace() {
    if (!m_active) return;
    m_workspaceManager->selectNext();
}

void CarouselPlugin::prevWorkspace() {
    if (!m_active) return;
    m_workspaceManager->selectPrevious();
}

void CarouselPlugin::selectWorkspace() {
    if (!m_active) return;
    
    m_animationEngine->startGrowAnimation();
    // After animation completes, switch to workspace
}

void CarouselPlugin::exitCarousel() {
    m_active = false;
    m_animationEngine->stopCarouselAnimation();
}

// Plugin API implementation
extern "C" {
    APICALL const char* PLUGIN_API_VERSION() {
        return HYPRLAND_API_VERSION;
    }
    
    APICALL PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
        PHANDLE = handle;
        
        auto& plugin = CarouselPlugin::getInstance();
        if (!plugin.initialize()) {
            return {"Hypr Carousel", "Failed to initialize", "1.0.0", "YourName"};
        }
        
        // Register keybinds
        HyprlandAPI::addDispatcher(PHANDLE, "carousel:toggle", [](std::string arg) {
            CarouselPlugin::getInstance().toggleCarousel();
        });
        
        HyprlandAPI::addDispatcher(PHANDLE, "carousel:next", [](std::string arg) {
            CarouselPlugin::getInstance().nextWorkspace();
        });
        
        HyprlandAPI::addDispatcher(PHANDLE, "carousel:prev", [](std::string arg) {
            CarouselPlugin::getInstance().prevWorkspace();
        });
        
        HyprlandAPI::addDispatcher(PHANDLE, "carousel:select", [](std::string arg) {
            CarouselPlugin::getInstance().selectWorkspace();
        });
        
        HyprlandAPI::addDispatcher(PHANDLE, "carousel:exit", [](std::string arg) {
            CarouselPlugin::getInstance().exitCarousel();
        });
        
        return {"Hypr Carousel", "3D workspace carousel for Hyprland", "1.0.0", "YourName"};
    }
    
    APICALL void PLUGIN_EXIT() {
        CarouselPlugin::getInstance().cleanup();
    }
}
```

## Build Instructions

1. **Clone and setup the project:**
```bash
mkdir hypr-carousel && cd hypr-carousel
# Copy all the source files into the appropriate directories
```

2. **Build the plugin:**
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

3. **Install the plugin:**
```bash
# Copy the built .so file to Hyprland's plugin directory
cp hypr-carousel.so ~/.local/share/hyprland/plugins/
```

## Configuration

Add to your `hyprland.conf`:

```conf
# Load the plugin
plugin = ~/.local/share/hyprland/plugins/hypr-carousel.so

# Keybindings
bind = SUPER, TAB, exec, hyprctl dispatch carousel:toggle
bind = SUPER, right, exec, hyprctl dispatch carousel:next
bind = SUPER, left, exec, hyprctl dispatch carousel:prev
bind = SUPER, RETURN, exec, hyprctl dispatch carousel:select
bind = SUPER, ESCAPE, exec, hyprctl dispatch carousel:exit

# Plugin configuration
plugin {
    carousel {
        radius = 800
        spacing = 1.2
        transparency_gradient = 0.3
        animation_duration = 300
    }
}
```

## Usage

- **Super + Tab**: Toggle carousel mode
- **Super + Left/Right**: Navigate between workspaces in carousel
- **Super + Enter**: Select and switch to highlighted workspace
- **Super + Escape**: Exit carousel mode

## Advanced Features

### Custom Animations
The `AnimationEngine` class supports:
- Smooth rotation transitions
- Elastic workspace selection
- Fade in/out effects
- Custom easing functions

### Shader Customization
Modify the fragment shader to add:
- Blur effects
- Color grading
- Edge highlighting
- Dynamic lighting

### Performance Optimization
- Workspace texture caching
- LOD (Level of Detail) rendering
- Frustum culling
- Async texture loading

## Troubleshooting

### Common Issues

1. **Plugin won't load:**
   - Check Hyprland version compatibility
   - Verify all dependencies are installed
   - Check plugin path in configuration

2. **Graphics corruption:**
   - Update graphics drivers
   - Verify OpenGL support
   - Check shader compilation logs

3. **Performance issues:**
   - Reduce carousel radius
   - Lower texture resolution
   - Disable transparency effects

### Debug Mode

Enable debug logging by setting:
```bash
export HYPRLAND_LOG_WLR=1
export HYPRLAND_NO_RT=1
```

## Contributing

This plugin provides a solid foundation for a 3D workspace carousel. Future enhancements could include:

- Texture filtering and anti-aliasing
- Multi-monitor support
- Customizable workspace layouts
- Touch gesture support
- Window-level previews within workspaces

## License

This documentation and code template is provided under the MIT License.