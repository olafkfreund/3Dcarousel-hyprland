#pragma once

// Hyprland 0.50.0 official headers with hyprland/ prefix for Nix
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/helpers/Color.hpp>

#include <memory>
#include <functional>

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

// Plugin entry points (camelCase format required by Hyprland)
extern "C" {
    APICALL EXPORT const char* pluginAPIVersion();
    APICALL EXPORT PLUGIN_DESCRIPTION_INFO pluginInit(HANDLE handle);
    APICALL EXPORT void pluginExit();
}