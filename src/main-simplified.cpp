// Simplified 3D Carousel Plugin following 3D stack patterns
// Based on successful patterns from olafkfreund/3Dstack-hyrpland

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Workspace.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/desktop/Window.hpp>

#include <vector>
#include <memory>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global plugin handle
inline HANDLE PHANDLE = nullptr;

// Configuration structure (following 3D stack pattern)
struct CarouselConfig {
    float radius = 800.0f;
    float spacing = 1.2f;
    float transparencyGradient = 0.3f;
    int animationDuration = 300;
    bool enableDepthBlur = false;
    int maxWorkspaces = 10;
};

// Workspace representation for carousel
struct WorkspaceItem {
    SP<CWorkspace> workspace;
    Vector2D position;
    float angle;
    float scale;
    float alpha;
    bool isSelected;
    
    WorkspaceItem(SP<CWorkspace> ws) : workspace(ws), position(0, 0), angle(0.0f), scale(1.0f), alpha(1.0f), isSelected(false) {}
};

// Main plugin class (following 3D stack modular pattern)
class CarouselPlugin {
private:
    bool m_bActive = false;
    std::vector<WorkspaceItem> m_vWorkspaces;
    int m_iSelectedIndex = 0;
    float m_fRotationAngle = 0.0f;
    CarouselConfig m_config;
    
    // Event hooks (following 3D stack pattern)
    SP<HOOK_CALLBACK_FN> m_pWorkspaceChangeHook;
    SP<HOOK_CALLBACK_FN> m_pRenderHook;

public:
    static CarouselPlugin& getInstance() {
        static CarouselPlugin instance;
        return instance;
    }
    
    bool initialize() {
        loadConfiguration();
        setupHooks();
        HyprlandAPI::addNotification(PHANDLE, "[Carousel] Initialized successfully", 
                                   CHyprColor{0.2, 1.0, 0.2, 1.0}, 2000);
        return true;
    }
    
    void loadConfiguration() {
        // Load configuration from Hyprland config (following 3D stack pattern)
        try {
            auto* const PRADIUS = HyprlandAPI::getConfigValue(PHANDLE, "plugin:carousel:radius");
            if (PRADIUS && PRADIUS->getDataStaticPtr()) {
                m_config.radius = std::any_cast<Hyprlang::INT>(*PRADIUS->getDataStaticPtr());
            }
            
            auto* const PSPACING = HyprlandAPI::getConfigValue(PHANDLE, "plugin:carousel:spacing");
            if (PSPACING && PSPACING->getDataStaticPtr()) {
                m_config.spacing = std::any_cast<Hyprlang::FLOAT>(*PSPACING->getDataStaticPtr());
            }
            
            auto* const PALPHA = HyprlandAPI::getConfigValue(PHANDLE, "plugin:carousel:transparency_gradient");
            if (PALPHA && PALPHA->getDataStaticPtr()) {
                m_config.transparencyGradient = std::any_cast<Hyprlang::FLOAT>(*PALPHA->getDataStaticPtr());
            }
            
        } catch (...) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Using default configuration", 
                                       CHyprColor{1.0, 1.0, 0.2, 1.0}, 1000);
        }
    }
    
    void setupHooks() {
        // Setup event hooks (following 3D stack pattern)
        m_pRenderHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "render", [this](void*, SCallbackInfo&, std::any) {
            this->onRender();
        });
        
        m_pWorkspaceChangeHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "workspace", [this](void*, SCallbackInfo&, [[maybe_unused]] std::any data) {
            this->onWorkspaceChange();
        });
    }
    
    void toggleCarousel() {
        m_bActive = !m_bActive;
        
        if (m_bActive) {
            updateWorkspaceList();
            calculatePositions();
            HyprlandAPI::addNotification(PHANDLE, "Carousel activated: " + std::to_string(m_vWorkspaces.size()) + " workspaces", 
                                       CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
        } else {
            HyprlandAPI::addNotification(PHANDLE, "Carousel deactivated", 
                                       CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
        }
    }
    
    void nextWorkspace() {
        if (!m_bActive || m_vWorkspaces.empty()) return;
        
        m_iSelectedIndex = (m_iSelectedIndex + 1) % m_vWorkspaces.size();
        updateSelection();
        calculatePositions();
        
        HyprlandAPI::addNotification(PHANDLE, "Next: " + std::to_string(m_iSelectedIndex + 1), 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 500);
    }
    
    void prevWorkspace() {
        if (!m_bActive || m_vWorkspaces.empty()) return;
        
        m_iSelectedIndex = (m_iSelectedIndex - 1 + m_vWorkspaces.size()) % m_vWorkspaces.size();
        updateSelection();
        calculatePositions();
        
        HyprlandAPI::addNotification(PHANDLE, "Previous: " + std::to_string(m_iSelectedIndex + 1), 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 500);
    }
    
    void selectWorkspace() {
        if (!m_bActive || m_vWorkspaces.empty() || m_iSelectedIndex >= (int)m_vWorkspaces.size()) return;
        
        auto& selectedItem = m_vWorkspaces[m_iSelectedIndex];
        if (selectedItem.workspace) {
            // Switch to workspace using Hyprland API
            std::string cmd = "workspace " + std::to_string(selectedItem.workspace->m_id);
            HyprlandAPI::invokeHyprctlCommand("dispatch", cmd);
            
            HyprlandAPI::addNotification(PHANDLE, "Switched to workspace " + std::to_string(selectedItem.workspace->m_id), 
                                       CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
        }
        
        // Exit carousel after selection
        m_bActive = false;
    }
    
    void exitCarousel() {
        m_bActive = false;
        HyprlandAPI::addNotification(PHANDLE, "Carousel exited", 
                                   CHyprColor{1.0, 0.2, 0.2, 1.0}, 500);
    }

private:
    void updateWorkspaceList() {
        m_vWorkspaces.clear();
        
        if (!g_pCompositor) {
            // Create demo workspaces if compositor not available
            for (int i = 1; i <= 4; ++i) {
                WorkspaceItem item(nullptr);
                m_vWorkspaces.push_back(item);
            }
            return;
        }
        
        // Get all workspaces (following 3D stack pattern)
        for (auto& workspace : g_pCompositor->m_workspaces) {
            if (!workspace->m_isSpecialWorkspace) {
                WorkspaceItem item(workspace);
                m_vWorkspaces.push_back(item);
            }
        }
        
        // Ensure we have at least one workspace
        if (m_vWorkspaces.empty()) {
            WorkspaceItem item(nullptr);
            m_vWorkspaces.push_back(item);
        }
        
        // Find current workspace
        if (g_pCompositor->m_lastMonitor) {
            auto currentWS = g_pCompositor->getWorkspaceByID(g_pCompositor->m_lastMonitor->activeWorkspaceID());
            for (size_t i = 0; i < m_vWorkspaces.size(); ++i) {
                if (m_vWorkspaces[i].workspace == currentWS) {
                    m_iSelectedIndex = i;
                    break;
                }
            }
        }
    }
    
    void updateSelection() {
        for (size_t i = 0; i < m_vWorkspaces.size(); ++i) {
            m_vWorkspaces[i].isSelected = (i == (size_t)m_iSelectedIndex);
        }
    }
    
    void calculatePositions() {
        if (m_vWorkspaces.empty()) return;
        
        // Calculate 3D carousel positions
        float centerX = 960.0f; // TODO: Get from monitor
        float centerY = 540.0f;
        
        m_fRotationAngle = (2.0f * M_PI * m_iSelectedIndex) / m_vWorkspaces.size();
        
        for (size_t i = 0; i < m_vWorkspaces.size(); ++i) {
            auto& item = m_vWorkspaces[i];
            
            // Calculate angle for this workspace
            float workspaceAngle = (2.0f * M_PI * i) / m_vWorkspaces.size() - m_fRotationAngle;
            item.angle = workspaceAngle;
            
            // 3D positioning with perspective
            float z = sinf(workspaceAngle) * m_config.radius;
            
            // Apply perspective scaling
            float perspective = 1.0f / (1.0f + fabsf(z) / 2000.0f);
            item.scale = perspective;
            
            // Final position
            item.position.x = centerX + cosf(workspaceAngle) * m_config.radius * perspective;
            item.position.y = centerY;
            
            // Alpha based on depth and selection
            if (item.isSelected) {
                item.alpha = 1.0f;
            } else {
                item.alpha = fmaxf(0.3f, 1.0f - fabsf(z) / m_config.radius * m_config.transparencyGradient);
            }
        }
    }
    
    void onRender() {
        if (!m_bActive || !g_pHyprOpenGL) return;
        
        static int renderCount = 0;
        renderCount++;
        
        // Debug first few renders
        if (renderCount <= 3) {
            std::string msg = "Render " + std::to_string(renderCount) + ": ";
            msg += std::to_string(m_vWorkspaces.size()) + " workspaces, ";
            msg += "OpenGL=" + std::string(g_pHyprOpenGL ? "OK" : "NULL");
            
            HyprlandAPI::addNotification(PHANDLE, msg, 
                                       CHyprColor{0.8, 0.8, 0.2, 1.0}, 1000);
        }
        
        renderCarousel();
    }
    
    void onWorkspaceChange() {
        if (m_bActive) {
            updateWorkspaceList();
            calculatePositions();
        }
    }
    
    void renderCarousel() {
        try {
            // Semi-transparent background
            CHyprColor bgColor{0.1, 0.1, 0.1, 0.8};
            g_pHyprOpenGL->clear(bgColor);
            
            // Render each workspace
            for (auto& item : m_vWorkspaces) {
                renderWorkspaceItem(item);
            }
            
        } catch (...) {
            static int errors = 0;
            if (errors++ < 3) {
                HyprlandAPI::addNotification(PHANDLE, "Render error", 
                                           CHyprColor{1.0, 0.2, 0.2, 1.0}, 1000);
            }
        }
    }
    
    void renderWorkspaceItem(const WorkspaceItem& item) {
        if (!g_pHyprOpenGL) return;
        
        try {
            // Calculate box size based on scale
            float width = 400.0f * item.scale;
            float height = 300.0f * item.scale;
            
            CBox box{
                (double)(item.position.x - width / 2),
                (double)(item.position.y - height / 2),
                (double)width,
                (double)height
            };
            
            // Workspace color (varies by index or ID)
            int colorIndex = item.workspace ? item.workspace->m_id : (&item - &m_vWorkspaces[0]);
            float hue = (colorIndex * 60.0f) / 360.0f;
            CHyprColor workspaceColor{
                0.3f + 0.4f * sinf(hue * 2 * M_PI),
                0.3f + 0.4f * sinf((hue + 0.33f) * 2 * M_PI),
                0.3f + 0.4f * sinf((hue + 0.66f) * 2 * M_PI),
                item.alpha
            };
            
            // Render workspace
            g_pHyprOpenGL->renderRect(box, workspaceColor);
            
            // Selection indicator
            if (item.isSelected) {
                CHyprColor highlightColor{1.0, 0.8, 0.2, 1.0};
                CBox borderBox{
                    box.x - 8, box.y - 8,
                    box.width + 16, box.height + 16
                };
                g_pHyprOpenGL->renderRect(borderBox, highlightColor);
            }
            
        } catch (...) {
            // Silent fallback
        }
    }
};

// Test function
SDispatchResult carousel_test(std::string) {
    std::string status = "Simplified test: ";
    
    status += "Compositor=" + std::string(g_pCompositor ? "OK" : "NULL") + " ";
    status += "OpenGL=" + std::string(g_pHyprOpenGL ? "OK" : "NULL") + " ";
    status += "Renderer=" + std::string(g_pHyprRenderer ? "OK" : "NULL");
    
    HyprlandAPI::addNotification(PHANDLE, status, 
                               CHyprColor{0.2, 0.8, 1.0, 1.0}, 3000);
    return SDispatchResult{};
}

// Dispatcher functions
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

// Plugin entry points
extern "C" {
    APICALL EXPORT std::string PLUGIN_API_VERSION() {
        return HYPRLAND_API_VERSION;
    }
    
    APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
        PHANDLE = handle;
        
        // Version check
        const std::string HASH = __hyprland_api_get_hash();
        if (HASH != GIT_COMMIT_HASH) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Version mismatch", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            throw std::runtime_error("Version mismatch");
        }
        
        // Register configuration options (following 3D stack pattern)
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:radius", 
                                   Hyprlang::CConfigValue((Hyprlang::INT)800));
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:spacing", 
                                   Hyprlang::CConfigValue((Hyprlang::FLOAT)1.2f));
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:transparency_gradient", 
                                   Hyprlang::CConfigValue((Hyprlang::FLOAT)0.3f));
        HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:animation_duration", 
                                   Hyprlang::CConfigValue((Hyprlang::INT)300));
        
        // Initialize plugin
        auto& plugin = CarouselPlugin::getInstance();
        if (!plugin.initialize()) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Failed to initialize", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            return {"Hypr Carousel", "Failed to initialize", "Claude", "1.0.0"};
        }
        
        // Register dispatchers
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:test", carousel_test);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:toggle", carousel_toggle);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:next", carousel_next);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:prev", carousel_prev);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:select", carousel_select);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:exit", carousel_exit);
        
        HyprlandAPI::addNotification(PHANDLE, "[Carousel] Simplified plugin loaded successfully!", 
                                   CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);
        
        return {"Hypr Carousel", "3D workspace carousel (simplified)", "Claude", "1.0.0"};
    }
    
    APICALL EXPORT void PLUGIN_EXIT() {
        CarouselPlugin::getInstance().exitCarousel();
    }
}