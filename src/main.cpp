#include "../include/plugin.hpp"
#include "CarouselRenderer.hpp"
#include "WorkspaceManager.hpp"
#include "AnimationEngine.hpp"

// Global plugin handle (required by Hyprland plugin system)
inline HANDLE PHANDLE = nullptr;

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
        HyprlandAPI::addNotification(PHANDLE, "Failed to initialize renderer", CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{1.0, 0.0, 0.0}), 1.0), 3000);
        return false;
    }
    
    if (!m_workspaceManager->initialize()) {
        HyprlandAPI::addNotification(PHANDLE, "Failed to initialize workspace manager", CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{1.0, 0.0, 0.0}), 1.0), 3000);
        return false;
    }
    
    if (!m_animationEngine->initialize()) {
        HyprlandAPI::addNotification(PHANDLE, "Failed to initialize animation engine", CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{1.0, 0.0, 0.0}), 1.0), 3000);
        return false;
    }
    
    m_initialized = true;
    return true;
}

void CarouselPlugin::cleanup() {
    if (m_renderer) m_renderer->cleanup();
    if (m_workspaceManager) m_workspaceManager->cleanup();
    if (m_animationEngine) m_animationEngine->cleanup();
    
    m_renderer.reset();
    m_workspaceManager.reset();
    m_animationEngine.reset();
    
    m_initialized = false;
}

void CarouselPlugin::toggleCarousel() {
    if (!m_initialized) return;
    
    m_active = !m_active;
    
    if (m_active) {
        m_renderer->captureWorkspaces();
        m_animationEngine->startCarouselAnimation();
        HyprlandAPI::addNotification(PHANDLE, "Carousel activated", CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{0.0, 1.0, 0.0}), 1.0), 1000);
    } else {
        m_animationEngine->stopCarouselAnimation();
        HyprlandAPI::addNotification(PHANDLE, "Carousel deactivated", CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{0.0, 1.0, 0.0}), 1.0), 1000);
    }
}

void CarouselPlugin::nextWorkspace() {
    if (!m_active || !m_initialized) return;
    m_workspaceManager->selectNext();
    
    float rotation = m_workspaceManager->getCurrentSelection() * (2.0f * M_PI / m_workspaceManager->getWorkspaceCount());
    m_renderer->updateWorkspacePositions(rotation, m_workspaceManager->getCurrentSelection());
}

void CarouselPlugin::prevWorkspace() {
    if (!m_active || !m_initialized) return;
    m_workspaceManager->selectPrevious();
    
    float rotation = m_workspaceManager->getCurrentSelection() * (2.0f * M_PI / m_workspaceManager->getWorkspaceCount());
    m_renderer->updateWorkspacePositions(rotation, m_workspaceManager->getCurrentSelection());
}

void CarouselPlugin::selectWorkspace() {
    if (!m_active || !m_initialized) return;
    
    m_animationEngine->startGrowAnimation();
    HyprlandAPI::addNotification(PHANDLE, "Workspace selected", CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{0.0, 1.0, 0.0}), 1.0), 1000);
}

void CarouselPlugin::exitCarousel() {
    if (!m_initialized) return;
    m_active = false;
    m_animationEngine->stopCarouselAnimation();
}

extern "C" {
    APICALL EXPORT const char* pluginAPIVersion() {
        return HYPRLAND_API_VERSION;
    }
    
    APICALL EXPORT PLUGIN_DESCRIPTION_INFO pluginInit(HANDLE handle) {
        PHANDLE = handle;
        
        // Version check (critical for stability)
        const std::string HASH = __hyprland_api_get_hash();
        if (HASH != GIT_COMMIT_HASH) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Version mismatch!", 
                                       CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{1.0, 0.2, 0.2}), 1.0), 5000);
            throw std::runtime_error("API version mismatch - plugin built for different Hyprland version");
        }
        
        auto& plugin = CarouselPlugin::getInstance();
        if (!plugin.initialize()) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Failed to initialize!", 
                                       CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{1.0, 0.2, 0.2}), 1.0), 5000);
            return {"Hypr Carousel", "Failed to initialize", "Claude", "1.0.0"};
        }
        
        // Register dispatchers
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:toggle", [](std::string) {
            CarouselPlugin::getInstance().toggleCarousel();
            return SDispatchResult{};
        });
        
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:next", [](std::string) {
            CarouselPlugin::getInstance().nextWorkspace();
            return SDispatchResult{};
        });
        
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:prev", [](std::string) {
            CarouselPlugin::getInstance().prevWorkspace();
            return SDispatchResult{};
        });
        
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:select", [](std::string) {
            CarouselPlugin::getInstance().selectWorkspace();
            return SDispatchResult{};
        });
        
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:exit", [](std::string) {
            CarouselPlugin::getInstance().exitCarousel();
            return SDispatchResult{};
        });
        
        HyprlandAPI::addNotification(PHANDLE, "[Carousel] Plugin loaded successfully!", 
                                   CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{0.0, 1.0, 0.0}), 1.0), 3000);
        
        return {"Hypr Carousel", "3D workspace carousel for Hyprland", "Claude", "1.0.0"};
    }
    
    APICALL EXPORT void pluginExit() {
        CarouselPlugin::getInstance().cleanup();
    }
}