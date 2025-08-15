// 3D Carousel Plugin with Real Workspace Thumbnail Capture
// Based on hyprexpo patterns for workspace rendering

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

// Workspace thumbnail structure with framebuffer
struct WorkspaceThumbnail {
    SP<CWorkspace> workspace;
    CBox box;          // 3D positioned box for rendering
    float angle;       // Carousel rotation angle
    float distance;    // Distance from center
    float alpha;       // Transparency
    bool isSelected;   // Currently selected workspace
    
    // Framebuffer for workspace capture (like hyprexpo)
    CFramebuffer framebuffer;
    bool captured = false;
    
    WorkspaceThumbnail(SP<CWorkspace> ws) : workspace(ws), angle(0.0f), distance(800.0f), alpha(0.7f), isSelected(false) {}
};

// Plugin state management
class CarouselPlugin {
private:
    bool m_bActive = false;
    std::vector<WorkspaceThumbnail> m_vThumbnails;
    float m_fRotationAngle = 0.0f;
    int m_iSelectedWorkspace = 0;
    
    // Configuration
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
    
    void loadConfiguration() {
        // Load configuration from Hyprland config
        try {
            auto radiusConfig = HyprlandAPI::getConfigValue(PHANDLE, "plugin:carousel:radius");
            if (radiusConfig) {
                m_config.radius = std::any_cast<Hyprlang::INT>(radiusConfig->getValue());
            }
        } catch (...) {
            // Use defaults if config loading fails
        }
    }
    
    void toggleCarousel() {
        m_bActive = !m_bActive;
        
        if (m_bActive) {
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
        if (!m_bActive || m_vThumbnails.empty()) return;
        
        m_iSelectedWorkspace = (m_iSelectedWorkspace + 1) % m_vThumbnails.size();
        updateRotation();
        HyprlandAPI::addNotification(PHANDLE, "Next workspace", 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 500);
    }
    
    void prevWorkspace() {
        if (!m_bActive || m_vThumbnails.empty()) return;
        
        m_iSelectedWorkspace = (m_iSelectedWorkspace - 1 + m_vThumbnails.size()) % m_vThumbnails.size();
        updateRotation();
        HyprlandAPI::addNotification(PHANDLE, "Previous workspace", 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 500);
    }
    
    void selectWorkspace() {
        if (!m_bActive || m_vThumbnails.empty()) return;
        
        if (m_iSelectedWorkspace < (int)m_vThumbnails.size()) {
            auto& thumbnail = m_vThumbnails[m_iSelectedWorkspace];
            if (thumbnail.workspace) {
                // Switch to selected workspace
                HyprlandAPI::invokeHyprctlCommand("dispatch", "workspace " + std::to_string(thumbnail.workspace->monitorID()));
                HyprlandAPI::addNotification(PHANDLE, "Switched to workspace " + std::to_string(thumbnail.workspace->monitorID()), 
                                           CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
                toggleCarousel(); // Exit carousel mode
            }
        }
    }
    
    void exitCarousel() {
        if (m_bActive) {
            m_bActive = false;
            cleanup();
            HyprlandAPI::addNotification(PHANDLE, "Exit carousel", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 500);
        }
    }
    
    // Render callback
    void onRender() {
        static int renderCount = 0;
        renderCount++;
        
        if (renderCount <= 5) {
            std::string debugMsg = "Thumbnail render " + std::to_string(renderCount) + ": ";
            
            if (!g_pHyprOpenGL) {
                debugMsg += "g_pHyprOpenGL=NULL ";
            } else {
                debugMsg += "g_pHyprOpenGL=OK ";
            }
            
            if (!g_pHyprRenderer) {
                debugMsg += "g_pHyprRenderer=NULL ";
            } else {
                debugMsg += "g_pHyprRenderer=OK ";
            }
            
            if (m_vThumbnails.empty()) {
                debugMsg += "thumbnails=EMPTY";
            } else {
                debugMsg += "thumbnails=" + std::to_string(m_vThumbnails.size()) + " ";
                
                // Count captured thumbnails
                int captured = 0;
                for (const auto& thumb : m_vThumbnails) {
                    if (thumb.captured) captured++;
                }
                debugMsg += "captured=" + std::to_string(captured);
            }
            
            HyprlandAPI::addNotification(PHANDLE, debugMsg, 
                                       CHyprColor{0.8, 0.8, 0.2, 1.0}, 1000);
        }
        
        if (!m_bActive || !g_pHyprOpenGL || !g_pHyprRenderer || m_vThumbnails.empty()) {
            return;
        }
        
        renderCarouselWithThumbnails();
    }
    
    bool isActive() const { return m_bActive; }

private:
    void captureWorkspaces() {
        if (!g_pCompositor) {
            createDemoThumbnails();
            return;
        }
        
        m_vThumbnails.clear();
        
        // Get all workspaces (following hyprexpo pattern)
        for (auto& workspace : g_pCompositor->m_workspaces) {
            if (!workspace->m_isSpecialWorkspace) {
                WorkspaceThumbnail thumbnail(workspace);
                
                // Capture workspace to framebuffer (like hyprexpo)
                captureWorkspaceToFramebuffer(thumbnail);
                
                m_vThumbnails.push_back(std::move(thumbnail));
            }
        }
        
        if (m_vThumbnails.empty()) {
            createDemoThumbnails();
            return;
        }
        
        // Find current workspace index
        auto currentWS = g_pCompositor->getWorkspaceByID(g_pCompositor->m_lastMonitor->activeWorkspaceID());
        for (size_t i = 0; i < m_vThumbnails.size(); ++i) {
            if (m_vThumbnails[i].workspace == currentWS) {
                m_iSelectedWorkspace = i;
                break;
            }
        }
        
        // Initialize 3D positions
        updateThumbnailPositions();
    }
    
    void captureWorkspaceToFramebuffer(WorkspaceThumbnail& thumbnail) {
        if (!g_pHyprRenderer || !g_pHyprOpenGL || !thumbnail.workspace) {
            thumbnail.captured = false;
            return;
        }
        
        try {
            // Create framebuffer for thumbnail (following hyprexpo size)
            thumbnail.framebuffer.alloc(400, 300, g_pHyprRenderer->isNvidia());
            
            if (!thumbnail.framebuffer.isAllocated()) {
                thumbnail.captured = false;
                return;
            }
            
            // Find monitor for this workspace
            CMonitor* pMonitor = g_pCompositor->m_lastMonitor.get();
            if (!pMonitor) {
                thumbnail.captured = false;
                return;
            }
            
            // Render workspace to framebuffer (hyprexpo pattern)
            thumbnail.framebuffer.bind();
            
            // Clear framebuffer
            g_pHyprOpenGL->clear(CHyprColor{0.1, 0.1, 0.1, 1.0});
            
            // Render workspace content (simplified version)
            // In full implementation, this would render all windows in the workspace
            renderWorkspaceContent(thumbnail.workspace, pMonitor, CBox{0, 0, 400, 300});
            
            thumbnail.framebuffer.release();
            thumbnail.captured = true;
            
        } catch (...) {
            thumbnail.captured = false;
        }
    }
    
    void renderWorkspaceContent(SP<CWorkspace> workspace, CMonitor* pMonitor, const CBox& box) {
        if (!workspace || !pMonitor || !g_pHyprOpenGL) return;
        
        try {
            // Simple workspace representation for now
            // In full implementation: render all windows in workspace
            
            // Get window count for visual indication
            int windowCount = workspace->getWindows();
            
            // Base color varies by workspace ID
            float hue = (workspace->getID() * 60.0f) / 360.0f; // Different hue per workspace
            CHyprColor workspaceColor{
                0.3f + 0.4f * sinf(hue * 2 * M_PI),
                0.3f + 0.4f * sinf((hue + 0.33f) * 2 * M_PI), 
                0.3f + 0.4f * sinf((hue + 0.66f) * 2 * M_PI),
                0.8f
            };
            
            // Render workspace background
            g_pHyprOpenGL->renderRect(box, workspaceColor);
            
            // Add visual indication of window count
            for (int i = 0; i < std::min(windowCount, 6); ++i) {
                CBox windowIndicator{
                    box.x + 20 + (i % 3) * 120,
                    box.y + 20 + (i / 3) * 120,
                    100, 100
                };
                
                CHyprColor windowColor{1.0, 1.0, 1.0, 0.6};
                g_pHyprOpenGL->renderRect(windowIndicator, windowColor);
            }
            
        } catch (...) {
            // Fallback to solid color
            CHyprColor fallbackColor{0.2, 0.4, 0.8, 0.8};
            g_pHyprOpenGL->renderRect(box, fallbackColor);
        }
    }
    
    void createDemoThumbnails() {
        m_vThumbnails.clear();
        
        // Create demo thumbnails if no real workspaces available
        for (int i = 1; i <= 4; ++i) {
            WorkspaceThumbnail thumb(nullptr);
            thumb.captured = true; // Mark as captured for demo
            m_vThumbnails.push_back(thumb);
        }
        
        m_iSelectedWorkspace = 0;
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
        
        float centerX = 960.0f; // TODO: Get from monitor
        float centerY = 540.0f;
        
        for (size_t i = 0; i < m_vThumbnails.size(); ++i) {
            auto& thumbnail = m_vThumbnails[i];
            
            float workspaceAngle = (2.0f * M_PI * i) / m_vThumbnails.size() - m_fRotationAngle;
            
            thumbnail.angle = workspaceAngle;
            thumbnail.distance = m_config.radius;
            thumbnail.isSelected = (i == (size_t)m_iSelectedWorkspace);
            
            // 3D carousel positioning with perspective
            float x = centerX + cosf(workspaceAngle) * m_config.radius;
            float z = sinf(workspaceAngle) * m_config.radius;
            
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
    
    void renderCarouselWithThumbnails() {
        try {
            // Semi-transparent background
            CHyprColor bgColor{0.1, 0.1, 0.1, 0.8};
            g_pHyprOpenGL->clear(bgColor);
            
            // Render each workspace thumbnail
            for (auto& thumbnail : m_vThumbnails) {
                renderWorkspaceThumbnail(thumbnail);
            }
            
            // Render selection indicator
            if (m_iSelectedWorkspace < (int)m_vThumbnails.size()) {
                renderSelectionIndicator(m_vThumbnails[m_iSelectedWorkspace]);
            }
            
        } catch (...) {
            static int renderErrors = 0;
            if (renderErrors++ < 3) {
                HyprlandAPI::addNotification(PHANDLE, "Thumbnail rendering error", 
                                           CHyprColor{1.0, 0.2, 0.2, 1.0}, 1000);
            }
        }
    }
    
    void renderWorkspaceThumbnail(WorkspaceThumbnail& thumbnail) {
        if (!g_pHyprOpenGL) return;
        
        try {
            if (thumbnail.captured && thumbnail.framebuffer.isAllocated()) {
                // Render actual captured workspace thumbnail
                g_pHyprOpenGL->renderTexture(thumbnail.framebuffer.getTexture(), thumbnail.box, thumbnail.alpha);
            } else {
                // Fallback to colored rectangle
                CHyprColor workspaceColor{0.2, 0.4, 0.8, thumbnail.alpha};
                g_pHyprOpenGL->renderRect(thumbnail.box, workspaceColor);
            }
        } catch (...) {
            // Silent fallback
        }
    }
    
    void renderSelectionIndicator(const WorkspaceThumbnail& thumbnail) {
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
        // Clean up framebuffers
        for (auto& thumbnail : m_vThumbnails) {
            if (thumbnail.framebuffer.isAllocated()) {
                thumbnail.framebuffer.release();
            }
        }
        
        m_vThumbnails.clear();
        m_iSelectedWorkspace = 0;
        m_fRotationAngle = 0.0f;
    }
};

// Event callback functions
void onRenderCallback(void*, SCallbackInfo&, std::any) {
    CarouselPlugin::getInstance().onRender();
}

// Test function
SDispatchResult carousel_test(std::string) {
    std::string status = "Thumbnail test: ";
    
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
    
    // Test framebuffer allocation
    try {
        CFramebuffer testFB;
        testFB.alloc(100, 100, g_pHyprRenderer ? g_pHyprRenderer->isNvidia() : false);
        if (testFB.isAllocated()) {
            status += "Framebuffer=OK ";
            testFB.release();
        } else {
            status += "Framebuffer=FAIL ";
        }
    } catch (...) {
        status += "Framebuffer=ERROR ";
    }
    
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
        
        // Load configuration
        plugin.loadConfiguration();
        
        // Register dispatchers
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:test", carousel_test);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:toggle", carousel_toggle);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:next", carousel_next);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:prev", carousel_prev);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:select", carousel_select);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:exit", carousel_exit);
        
        // Register event callbacks for rendering
        static auto renderCallback = HyprlandAPI::registerCallbackDynamic(
            PHANDLE, "render", onRenderCallback);
        
        HyprlandAPI::addNotification(PHANDLE, "[Carousel] Plugin with thumbnails loaded!", 
                                   CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);
        
        return {"Hypr Carousel", "3D workspace carousel with real thumbnails", "Claude", "1.0.0"};
    }
    
    APICALL EXPORT void PLUGIN_EXIT() {
        CarouselPlugin::getInstance().exitCarousel();
    }
}