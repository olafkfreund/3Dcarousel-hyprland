// Function Hook-Based 3D Carousel Plugin
// Uses hooks like hyprexpo instead of render callbacks for better timing

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Workspace.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/desktop/Window.hpp>

#include <chrono>
#include <vector>
#include <memory>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global plugin handle
inline HANDLE PHANDLE = nullptr;

// Hook tracking
static bool g_bCarouselActive = false;
static bool g_bHooksInstalled = false;

// Forward declarations
typedef void (*origRenderMonitor)(CMonitor* pMonitor);
static origRenderMonitor g_pOriginalRenderMonitor = nullptr;

// Workspace thumbnail structure
struct WorkspaceThumbnail {
    SP<CWorkspace> workspace;
    CBox box;
    float angle;
    float distance;
    float alpha;
    bool isSelected;
    
    WorkspaceThumbnail(SP<CWorkspace> ws) : workspace(ws), angle(0.0f), distance(800.0f), alpha(0.7f), isSelected(false) {}
};

// Plugin state management
class CarouselPlugin {
private:
    std::vector<WorkspaceThumbnail> m_vThumbnails;
    float m_fRotationAngle = 0.0f;
    int m_iSelectedWorkspace = 0;
    
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
        return true;
    }
    
    void toggleCarousel() {
        g_bCarouselActive = !g_bCarouselActive;
        
        if (g_bCarouselActive) {
            captureWorkspaces();
            HyprlandAPI::addNotification(PHANDLE, "Carousel activated with " + std::to_string(m_vThumbnails.size()) + " workspaces", 
                                       CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
        } else {
            cleanup();
            HyprlandAPI::addNotification(PHANDLE, "Carousel deactivated", 
                                       CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
        }
    }
    
    void nextWorkspace() {
        if (!g_bCarouselActive || m_vThumbnails.empty()) return;
        
        m_iSelectedWorkspace = (m_iSelectedWorkspace + 1) % m_vThumbnails.size();
        updateRotation();
        HyprlandAPI::addNotification(PHANDLE, "Next workspace", 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 500);
    }
    
    void prevWorkspace() {
        if (!g_bCarouselActive || m_vThumbnails.empty()) return;
        
        m_iSelectedWorkspace = (m_iSelectedWorkspace - 1 + m_vThumbnails.size()) % m_vThumbnails.size();
        updateRotation();
        HyprlandAPI::addNotification(PHANDLE, "Previous workspace", 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 500);
    }
    
    void selectWorkspace() {
        if (!g_bCarouselActive || m_vThumbnails.empty()) return;
        
        if (m_iSelectedWorkspace < (int)m_vThumbnails.size()) {
            auto& thumbnail = m_vThumbnails[m_iSelectedWorkspace];
            if (thumbnail.workspace) {
                HyprlandAPI::invokeHyprctlCommand("dispatch", "workspace " + std::to_string(thumbnail.workspace->monitorID()));
                HyprlandAPI::addNotification(PHANDLE, "Switched to workspace", 
                                           CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
                toggleCarousel(); // Exit carousel mode
            }
        }
    }
    
    void exitCarousel() {
        if (g_bCarouselActive) {
            g_bCarouselActive = false;
            HyprlandAPI::addNotification(PHANDLE, "Exit carousel", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 500);
        }
    }
    
    bool isActive() const { return g_bCarouselActive; }
    
    // Hook-based rendering (called from our render hook)
    void renderCarouselOverlay(CMonitor* pMonitor) {
        if (!g_bCarouselActive || m_vThumbnails.empty()) return;
        
        static int hookRenderCount = 0;
        hookRenderCount++;
        
        // Debug hook rendering
        if (hookRenderCount <= 5) {
            std::string debugMsg = "Hook render " + std::to_string(hookRenderCount) + ": ";
            
            if (!g_pHyprOpenGL) {
                debugMsg += "g_pHyprOpenGL=NULL ";
            } else {
                debugMsg += "g_pHyprOpenGL=OK ";
            }
            
            if (!pMonitor) {
                debugMsg += "pMonitor=NULL";
            } else {
                debugMsg += "pMonitor=OK";
            }
            
            HyprlandAPI::addNotification(PHANDLE, debugMsg, 
                                       CHyprColor{0.8, 0.8, 0.2, 1.0}, 1000);
        }
        
        // Render only if OpenGL is available
        if (!g_pHyprOpenGL || !pMonitor) return;
        
        // Hook-based rendering (following hyprexpo pattern)
        renderCarouselWithHooks(pMonitor);
    }

private:
    void captureWorkspaces() {
        if (!g_pCompositor) {
            // Create demo thumbnails if compositor not available
            m_vThumbnails.clear();
            for (int i = 1; i <= 4; ++i) {
                WorkspaceThumbnail thumb(nullptr);
                thumb.angle = (2.0f * M_PI * (i-1)) / 4.0f;
                thumb.distance = 800.0f;
                thumb.alpha = (i == 1) ? 1.0f : 0.7f;
                thumb.isSelected = (i == 1);
                
                // Calculate 3D position
                float centerX = 960.0f;
                float centerY = 540.0f;
                float x = centerX + cosf(thumb.angle) * thumb.distance;
                float z = sinf(thumb.angle) * thumb.distance;
                float perspective = 1.0f / (1.0f + z / 2000.0f);
                
                thumb.box = CBox{
                    (double)(x - 200 * perspective),
                    (double)(centerY - 150 * perspective),
                    (double)(400 * perspective),
                    (double)(300 * perspective)
                };
                
                m_vThumbnails.push_back(thumb);
            }
            m_iSelectedWorkspace = 0;
            return;
        }
        
        m_vThumbnails.clear();
        
        // Get all workspaces with windows
        for (auto& workspace : g_pCompositor->m_workspaces) {
            if (!workspace->m_isSpecialWorkspace && workspace->getWindows() > 0) {
                m_vThumbnails.emplace_back(workspace);
            }
        }
        
        if (m_vThumbnails.empty()) {
            // Create at least one demo thumbnail
            WorkspaceThumbnail thumb(nullptr);
            m_vThumbnails.push_back(thumb);
        }
        
        // Initialize 3D positions
        updateThumbnailPositions();
    }
    
    void updateRotation() {
        if (!m_vThumbnails.empty()) {
            m_fRotationAngle = (2.0f * M_PI * m_iSelectedWorkspace) / m_vThumbnails.size();
            updateThumbnailPositions();
        }
    }
    
    void updateThumbnailPositions() {
        if (m_vThumbnails.empty()) return;
        
        float centerX = 960.0f; // Hardcoded for now
        float centerY = 540.0f;
        
        for (size_t i = 0; i < m_vThumbnails.size(); ++i) {
            auto& thumbnail = m_vThumbnails[i];
            
            float workspaceAngle = (2.0f * M_PI * i) / m_vThumbnails.size() - m_fRotationAngle;
            
            thumbnail.angle = workspaceAngle;
            thumbnail.distance = m_config.radius;
            thumbnail.isSelected = (i == (size_t)m_iSelectedWorkspace);
            
            // 3D carousel positioning
            float x = centerX + cosf(workspaceAngle) * m_config.radius;
            float z = sinf(workspaceAngle) * m_config.radius;
            
            // Perspective projection
            float perspective = 1.0f / (1.0f + z / 2000.0f);
            float scaledRadius = m_config.radius * perspective;
            
            x = centerX + cosf(workspaceAngle) * scaledRadius;
            float y = centerY;
            
            float thumbnailWidth = 400.0f * perspective;
            float thumbnailHeight = 300.0f * perspective;
            
            thumbnail.box = CBox{
                (double)(x - thumbnailWidth / 2),
                (double)(y - thumbnailHeight / 2),
                (double)thumbnailWidth,
                (double)thumbnailHeight
            };
            
            // Alpha based on depth and selection
            if (thumbnail.isSelected) {
                thumbnail.alpha = 1.0f;
            } else {
                float depthAlpha = 1.0f - fabsf(z) / m_config.radius * m_config.transparencyGradient;
                thumbnail.alpha = fmaxf(0.3f, depthAlpha);
            }
        }
    }
    
    void renderCarouselWithHooks(CMonitor* pMonitor) {
        try {
            // Clear background like hyprexpo does
            CHyprColor bgColor{0.1, 0.1, 0.1, 0.8};
            g_pHyprOpenGL->clear(bgColor);
            
            // Render each workspace thumbnail
            for (auto& thumbnail : m_vThumbnails) {
                renderWorkspaceThumbnailHook(thumbnail);
            }
            
            // Render selection indicator
            if (m_iSelectedWorkspace < (int)m_vThumbnails.size()) {
                renderSelectionIndicatorHook(m_vThumbnails[m_iSelectedWorkspace]);
            }
            
        } catch (...) {
            static int renderErrors = 0;
            if (renderErrors++ < 3) {
                HyprlandAPI::addNotification(PHANDLE, "Hook rendering error", 
                                           CHyprColor{1.0, 0.2, 0.2, 1.0}, 1000);
            }
        }
    }
    
    void renderWorkspaceThumbnailHook(const WorkspaceThumbnail& thumbnail) {
        if (!g_pHyprOpenGL) return;
        
        try {
            CHyprColor workspaceColor{0.2, 0.4, 0.8, thumbnail.alpha};
            g_pHyprOpenGL->renderRect(thumbnail.box, workspaceColor);
        } catch (...) {
            // Silent fallback
        }
    }
    
    void renderSelectionIndicatorHook(const WorkspaceThumbnail& thumbnail) {
        if (!g_pHyprOpenGL) return;
        
        try {
            CHyprColor highlightColor{1.0, 0.8, 0.2, 1.0};
            
            CBox borderBox = {
                thumbnail.box.x - 8, 
                thumbnail.box.y - 8, 
                thumbnail.box.width + 16, 
                thumbnail.box.height + 16
            };
            
            g_pHyprOpenGL->renderRect(borderBox, highlightColor);
        } catch (...) {
            // Silent fallback
        }
    }
    
    void cleanup() {
        m_vThumbnails.clear();
        m_iSelectedWorkspace = 0;
        m_fRotationAngle = 0.0f;
    }
};

// Hook implementation (following hyprexpo pattern)
void hkRenderMonitor(CMonitor* pMonitor) {
    // Call original render function first
    if (g_pOriginalRenderMonitor) {
        g_pOriginalRenderMonitor(pMonitor);
    }
    
    // Then render our carousel overlay
    CarouselPlugin::getInstance().renderCarouselOverlay(pMonitor);
}

// Install function hooks
void installHooks() {
    if (g_bHooksInstalled) return;
    
    static auto RENDERMONTORPTR = HyprlandAPI::getFunctionAddressFromSignature(PHANDLE, "_ZN8CMonitor6renderEv");
    
    if (RENDERMONTORPTR) {
        g_pOriginalRenderMonitor = (origRenderMonitor)HyprlandAPI::createFunctionHook(PHANDLE, RENDERMONTORPTR, (void*)&hkRenderMonitor);
        
        if (g_pOriginalRenderMonitor && HyprlandAPI::enableFunctionHook(PHANDLE, RENDERMONTORPTR)) {
            g_bHooksInstalled = true;
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Hooks installed successfully", 
                                       CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
        } else {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Failed to install hooks", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 2000);
        }
    } else {
        HyprlandAPI::addNotification(PHANDLE, "[Carousel] Could not find render function", 
                                   CHyprColor{1.0, 0.2, 0.2, 1.0}, 2000);
    }
}

// Test function to check states
SDispatchResult carousel_test(std::string) {
    std::string status = "Hook-based test: ";
    
    if (!g_pCompositor) {
        status += "g_pCompositor=NULL ";
    } else {
        status += "g_pCompositor=OK ";
    }
    
    if (!g_pHyprOpenGL) {
        status += "g_pHyprOpenGL=NULL ";
    } else {
        status += "g_pHyprOpenGL=OK ";
    }
    
    if (!g_pHyprRenderer) {
        status += "g_pHyprRenderer=NULL ";
    } else {
        status += "g_pHyprRenderer=OK ";
    }
    
    status += "Hooks=" + std::string(g_bHooksInstalled ? "OK" : "FAIL");
    
    HyprlandAPI::addNotification(PHANDLE, status, 
                               CHyprColor{0.2, 0.8, 1.0, 1.0}, 4000);
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
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Version mismatch!", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            throw std::runtime_error("[carousel] Version mismatch");
        }
        
        // Initialize plugin
        auto& plugin = CarouselPlugin::getInstance();
        if (!plugin.initialize()) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Failed to initialize!", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            return {"Hypr Carousel", "Failed to initialize", "Claude", "1.0.0"};
        }
        
        // Install function hooks for rendering
        installHooks();
        
        // Register dispatchers
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:test", carousel_test);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:toggle", carousel_toggle);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:next", carousel_next);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:prev", carousel_prev);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:select", carousel_select);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:exit", carousel_exit);
        
        HyprlandAPI::addNotification(PHANDLE, "[Carousel] Hook-based plugin loaded!", 
                                   CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);
        
        return {"Hypr Carousel", "3D workspace carousel with function hooks", "Claude", "1.0.0"};
    }
    
    APICALL EXPORT void PLUGIN_EXIT() {
        CarouselPlugin::getInstance().exitCarousel();
    }
}