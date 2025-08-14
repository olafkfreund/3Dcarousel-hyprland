// Improved 3D Carousel Plugin following Hyprspace patterns
// Based on analysis of successful Hyprland plugins

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Workspace.hpp>
#include <hyprland/src/render/Renderer.hpp>

#include <vector>
#include <memory>

// Global plugin handle
inline HANDLE PHANDLE = nullptr;

// Plugin state management following Hyprspace patterns
class CarouselPlugin {
private:
    bool m_bActive = false;
    bool m_bSwiping = false;
    std::vector<SP<CWorkspace>> m_vWorkspaces;
    float m_fRotationAngle = 0.0f;
    int m_iSelectedWorkspace = 0;
    
    // Configuration (like Hyprspace)
    struct Config {
        float radius = 800.0f;
        float spacing = 1.2f;
        float transparencyGradient = 0.3f;
        int animationDuration = 300;
    } m_config;

public:
    static CarouselPlugin& getInstance() {
        static CarouselPlugin instance;
        return instance;
    }
    
    bool initialize() {
        if (!g_pCompositor) {
            return false;
        }
        
        loadConfiguration();
        return true;
    }
    
    void loadConfiguration() {
        // Load configuration values (following Hyprspace pattern)
        auto radiusConfig = HyprlandAPI::getConfigValue(PHANDLE, "plugin:carousel:radius");
        if (radiusConfig) {
            m_config.radius = std::any_cast<Hyprlang::INT>(radiusConfig->getValue());
        }
    }
    
    void toggleCarousel() {
        if (!g_pCompositor || !g_pCompositor->m_lastMonitor) {
            return;
        }
        
        m_bActive = !m_bActive;
        
        if (m_bActive) {
            captureWorkspaces();
            HyprlandAPI::addNotification(PHANDLE, "Carousel activated", 
                                       CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{0.0, 1.0, 0.0}), 1.0), 1000);
        } else {
            cleanup();
            HyprlandAPI::addNotification(PHANDLE, "Carousel deactivated", 
                                       CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{0.0, 1.0, 0.0}), 1.0), 1000);
        }
    }
    
    void nextWorkspace() {
        if (!m_bActive || m_vWorkspaces.empty()) return;
        
        m_iSelectedWorkspace = (m_iSelectedWorkspace + 1) % m_vWorkspaces.size();
        updateRotation();
    }
    
    void prevWorkspace() {
        if (!m_bActive || m_vWorkspaces.empty()) return;
        
        m_iSelectedWorkspace = (m_iSelectedWorkspace - 1 + m_vWorkspaces.size()) % m_vWorkspaces.size();
        updateRotation();
    }
    
    void selectWorkspace() {
        if (!m_bActive || m_vWorkspaces.empty()) return;
        
        // Switch to selected workspace
        if (m_iSelectedWorkspace < (int)m_vWorkspaces.size()) {
            auto workspace = m_vWorkspaces[m_iSelectedWorkspace];
            if (workspace) {
                // Use proper workspace switching API  
                HyprlandAPI::invokeHyprctlCommand("dispatch", "workspace " + std::to_string(workspace->monitorID()));
                toggleCarousel(); // Exit carousel mode
            }
        }
    }
    
    void exitCarousel() {
        if (m_bActive) {
            m_bActive = false;
            cleanup();
        }
    }
    
    // Event handlers following Hyprspace pattern
    void onRender() {
        if (!m_bActive || !g_pCompositor || !g_pCompositor->m_lastMonitor) {
            return;
        }
        
        renderCarousel();
    }
    
    bool isActive() const { return m_bActive; }

private:
    void captureWorkspaces() {
        if (!g_pCompositor) return;
        
        m_vWorkspaces.clear();
        
        // Get all workspaces with windows (following safe patterns)
        for (auto& workspace : g_pCompositor->m_workspaces) {
            if (!workspace->m_isSpecialWorkspace && workspace->getWindows() > 0) {
                m_vWorkspaces.push_back(workspace);
            }
        }
        
        // Find current workspace index
        auto currentWS = g_pCompositor->getWorkspaceByID(g_pCompositor->m_lastMonitor->activeWorkspaceID());
        for (size_t i = 0; i < m_vWorkspaces.size(); ++i) {
            if (m_vWorkspaces[i] == currentWS) {
                m_iSelectedWorkspace = i;
                break;
            }
        }
    }
    
    void updateRotation() {
        // Calculate target rotation angle
        if (!m_vWorkspaces.empty()) {
            m_fRotationAngle = (2.0f * M_PI * m_iSelectedWorkspace) / m_vWorkspaces.size();
        }
    }
    
    void renderCarousel() {
        // Safe rendering following Hyprspace patterns
        if (!g_pHyprOpenGL || !g_pCompositor->m_lastMonitor) {
            return;
        }
        
        auto monitor = g_pCompositor->m_lastMonitor;
        
        // TODO: Implement 3D carousel rendering using Hyprland's OpenGL context
        // This should render workspace thumbnails in 3D space
        // Following the pattern from Hyprspace's renderWorkspaceStub()
    }
    
    void cleanup() {
        m_vWorkspaces.clear();
        m_iSelectedWorkspace = 0;
        m_fRotationAngle = 0.0f;
    }
};

// Event callback functions following Hyprspace pattern
void onRenderCallback(void*, SCallbackInfo&, std::any) {
    CarouselPlugin::getInstance().onRender();
}

// Dispatcher functions with modern API
SDispatchResult carousel_toggle(std::string) {
    CarouselPlugin::getInstance().toggleCarousel();
    return SDispatchResult{};
}

SDispatchResult carousel_next(std::string) {
    CarouselPlugin::getInstance().nextWorkspace();
    return SDispatchResult{};
}

SDispatchResult carousel_prev(std::string) {
    CarouselPlugin::getInstance().prevWorkspace();
    return SDispatchResult{};
}

SDispatchResult carousel_select(std::string) {
    CarouselPlugin::getInstance().selectWorkspace();
    return SDispatchResult{};
}

SDispatchResult carousel_exit(std::string) {
    CarouselPlugin::getInstance().exitCarousel();
    return SDispatchResult{};
}

// Plugin entry points (exact naming required by Hyprland)
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
        
        // Initialize plugin
        auto& plugin = CarouselPlugin::getInstance();
        if (!plugin.initialize()) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Failed to initialize!", 
                                       CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{1.0, 0.2, 0.2}), 1.0), 5000);
            return {"Hypr Carousel", "Failed to initialize", "Claude", "1.0.0"};
        }
        
        // Register configuration values (following Hyprspace pattern)
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:radius", 
                                   Hyprlang::CConfigValue((Hyprlang::INT)800));
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:spacing", 
                                   Hyprlang::CConfigValue((Hyprlang::FLOAT)1.2f));
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:transparency_gradient", 
                                   Hyprlang::CConfigValue((Hyprlang::FLOAT)0.3f));
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:animation_duration", 
                                   Hyprlang::CConfigValue((Hyprlang::INT)300));
        
        // Register dispatchers using modern API
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:toggle", carousel_toggle);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:next", carousel_next);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:prev", carousel_prev);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:select", carousel_select);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:exit", carousel_exit);
        
        // Register event callbacks (following Hyprspace pattern)
        static auto renderCallback = HyprlandAPI::registerCallbackDynamic(
            PHANDLE, "render", onRenderCallback);
        
        HyprlandAPI::addNotification(PHANDLE, "[Carousel] Plugin loaded successfully!", 
                                   CHyprColor(Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{0.0, 1.0, 0.0}), 1.0), 3000);
        
        return {"Hypr Carousel", "3D workspace carousel following Hyprspace patterns", "Claude", "1.0.0"};
    }
    
    APICALL EXPORT void pluginExit() {
        // Clean exit
        CarouselPlugin::getInstance().exitCarousel();
    }
}