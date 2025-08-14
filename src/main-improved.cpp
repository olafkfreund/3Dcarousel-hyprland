// Improved 3D Carousel Plugin following Hyprspace patterns
// Based on analysis of successful Hyprland plugins

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

// Workspace thumbnail structure (following Hyprspace patterns)
struct WorkspaceThumbnail {
    SP<CWorkspace> workspace;
    CBox box;          // Rendered position and size
    float angle;       // 3D rotation angle
    float distance;    // Distance from center
    float alpha;       // Transparency
    bool isSelected;   // Currently selected workspace
    
    WorkspaceThumbnail(SP<CWorkspace> ws) : workspace(ws), angle(0.0f), distance(800.0f), alpha(0.7f), isSelected(false) {}
};

// Plugin state management following Hyprspace patterns
class CarouselPlugin {
private:
    bool m_bActive = false;
    bool m_bSwiping = false;
    std::vector<WorkspaceThumbnail> m_vThumbnails;
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
        // Simplified initialization - just return true
        return true;
    }
    
    void loadConfiguration() {
        // Simplified configuration loading - use defaults if config fails
        try {
            auto radiusConfig = HyprlandAPI::getConfigValue(PHANDLE, "plugin:carousel:radius");
            if (radiusConfig) {
                m_config.radius = std::any_cast<Hyprlang::INT>(radiusConfig->getValue());
            }
        } catch (...) {
            // Use defaults if config loading fails
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Using default config", 
                                       CHyprColor{1.0, 1.0, 0.2, 1.0}, 1000);
        }
    }
    
    void toggleCarousel() {
        m_bActive = !m_bActive;
        
        if (m_bActive) {
            // Simple carousel activation without relying on g_pCompositor
            // Use hyprctl to get workspace info instead
            auto workspaceList = HyprlandAPI::invokeHyprctlCommand("get", "workspaces");
            if (workspaceList.empty()) {
                HyprlandAPI::addNotification(PHANDLE, "Cannot get workspace info via hyprctl", 
                                           CHyprColor{1.0, 0.5, 0.2, 1.0}, 2000);
            } else {
                // For now, just create dummy thumbnails for demo
                m_vThumbnails.clear();
                for (int i = 1; i <= 3; ++i) {  // Create 3 dummy workspaces
                    WorkspaceThumbnail thumb(nullptr);
                    thumb.angle = (2.0f * M_PI * (i-1)) / 3.0f;
                    thumb.distance = 800.0f;
                    thumb.alpha = (i == 1) ? 1.0f : 0.7f;
                    thumb.isSelected = (i == 1);
                    
                    // Calculate position
                    float centerX = 960.0f; // Half of 1920
                    float centerY = 540.0f; // Half of 1080
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
                HyprlandAPI::addNotification(PHANDLE, "Carousel activated (demo mode) with " + std::to_string(m_vThumbnails.size()) + " workspaces", 
                                           CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
            }
        } else {
            cleanup();
            HyprlandAPI::addNotification(PHANDLE, "Carousel deactivated", 
                                       CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
        }
    }
    
    void nextWorkspace() {
        if (!m_bActive || m_vThumbnails.empty()) {
            HyprlandAPI::addNotification(PHANDLE, "Carousel not active or no workspaces", 
                                       CHyprColor{1.0, 0.5, 0.2, 1.0}, 1000);
            return;
        }
        
        m_iSelectedWorkspace = (m_iSelectedWorkspace + 1) % m_vThumbnails.size();
        updateRotation();
        HyprlandAPI::addNotification(PHANDLE, "Next workspace", 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 500);
    }
    
    void prevWorkspace() {
        if (!m_bActive || m_vThumbnails.empty()) {
            HyprlandAPI::addNotification(PHANDLE, "Carousel not active or no workspaces", 
                                       CHyprColor{1.0, 0.5, 0.2, 1.0}, 1000);
            return;
        }
        
        m_iSelectedWorkspace = (m_iSelectedWorkspace - 1 + m_vThumbnails.size()) % m_vThumbnails.size();
        updateRotation();
        HyprlandAPI::addNotification(PHANDLE, "Previous workspace", 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 500);
    }
    
    void selectWorkspace() {
        if (!m_bActive || m_vThumbnails.empty()) return;
        
        // Switch to selected workspace
        if (m_iSelectedWorkspace < (int)m_vThumbnails.size()) {
            auto& thumbnail = m_vThumbnails[m_iSelectedWorkspace];
            if (thumbnail.workspace) {
                // Use proper workspace switching API  
                HyprlandAPI::invokeHyprctlCommand("dispatch", "workspace " + std::to_string(thumbnail.workspace->monitorID()));
                HyprlandAPI::addNotification(PHANDLE, "Switched to workspace", 
                                           CHyprColor{0.2, 1.0, 0.2, 1.0}, 1000);
                toggleCarousel(); // Exit carousel mode
            }
        }
    }
    
    void exitCarousel() {
        if (m_bActive) {
            m_bActive = false;
            HyprlandAPI::addNotification(PHANDLE, "Exit carousel", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 500);
        }
    }
    
    // Event handlers following Hyprspace pattern
    void onRender() {
        static int onRenderCount = 0;
        onRenderCount++;
        
        if (onRenderCount < 5) { // Log first 5 onRender calls
            std::string msg = "onRender " + std::to_string(onRenderCount) + ": ";
            msg += m_bActive ? "ACTIVE " : "INACTIVE ";
            msg += std::to_string(m_vThumbnails.size()) + " thumbnails";
            
            HyprlandAPI::addNotification(PHANDLE, msg, 
                                       CHyprColor{0.2, 0.8, 0.8, 1.0}, 800);
        }
        
        if (!m_bActive || m_vThumbnails.empty()) {
            return;
        }
        
        renderCarousel();
    }
    
    bool isActive() const { return m_bActive; }

private:
    void captureWorkspaces() {
        if (!g_pCompositor) return;
        
        m_vThumbnails.clear();
        
        // Get all workspaces with windows (following safe patterns)
        for (auto& workspace : g_pCompositor->m_workspaces) {
            if (!workspace->m_isSpecialWorkspace && workspace->getWindows() > 0) {
                m_vThumbnails.emplace_back(workspace);
            }
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
    
    void updateRotation() {
        // Calculate target rotation angle for smooth 3D rotation
        if (!m_vThumbnails.empty()) {
            m_fRotationAngle = (2.0f * M_PI * m_iSelectedWorkspace) / m_vThumbnails.size();
            updateThumbnailPositions();
        }
    }
    
    void updateThumbnailPositions() {
        if (m_vThumbnails.empty() || !g_pCompositor->m_lastMonitor) return;
        
        auto monitor = g_pCompositor->m_lastMonitor;
        // TODO: Fix monitor size API - hardcode for now  
        float centerX = 1920.0f / 2.0f; // monitor->vecSize.x / 2.0f;
        float centerY = 1080.0f / 2.0f; // monitor->vecSize.y / 2.0f;
        
        for (size_t i = 0; i < m_vThumbnails.size(); ++i) {
            auto& thumbnail = m_vThumbnails[i];
            
            // Calculate angle for this workspace in the carousel
            float workspaceAngle = (2.0f * M_PI * i) / m_vThumbnails.size() - m_fRotationAngle;
            
            // 3D carousel positioning
            thumbnail.angle = workspaceAngle;
            thumbnail.distance = m_config.radius;
            thumbnail.isSelected = (i == (size_t)m_iSelectedWorkspace);
            
            // Calculate 3D position (project 3D circle to 2D screen)
            float x = centerX + cosf(workspaceAngle) * m_config.radius;
            float z = sinf(workspaceAngle) * m_config.radius; // Depth
            
            // Apply perspective projection (simple depth scaling)
            float perspective = 1.0f / (1.0f + z / 2000.0f); // Perspective factor
            float scaledRadius = m_config.radius * perspective;
            
            x = centerX + cosf(workspaceAngle) * scaledRadius;
            float y = centerY;
            
            // Thumbnail size based on distance (perspective scaling)
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
    
    void renderCarousel() {
        // Debug rendering calls
        static int renderCount = 0;
        renderCount++;
        
        if (renderCount < 10) { // Only log first 10 render calls
            std::string debugMsg = "Render call " + std::to_string(renderCount) + ": ";
            
            // Check all global pointers like official plugins do
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
                debugMsg += "thumbnails=" + std::to_string(m_vThumbnails.size());
            }
            
            HyprlandAPI::addNotification(PHANDLE, debugMsg, 
                                       CHyprColor{0.8, 0.8, 0.2, 1.0}, 1000);
        }
        
        // Wait for both OpenGL and Renderer to be available (like official plugins)
        if (!g_pHyprOpenGL || !g_pHyprRenderer || m_vThumbnails.empty()) {
            return;
        }
        
        // Official plugin pattern: render each workspace using proper API
        renderCarouselWithProperAPI();
    }
    
    void renderCarouselWithProperAPI() {
        // Following hyprexpo pattern: use g_pHyprRenderer for workspace rendering
        // and g_pHyprOpenGL for compositing
        
        try {
            // Clear background (following hyprexpo pattern)
            CHyprColor bgColor{0.1, 0.1, 0.1, 0.8}; // Semi-transparent dark background
            g_pHyprOpenGL->clear(bgColor);
            
            // Render each workspace thumbnail using proper API
            for (auto& thumbnail : m_vThumbnails) {
                renderWorkspaceThumbnailProper(thumbnail);
            }
            
            // Add selection indicator (following hyprexpo pattern)
            if (m_iSelectedWorkspace < (int)m_vThumbnails.size()) {
                renderSelectionIndicatorProper(m_vThumbnails[m_iSelectedWorkspace]);
            }
            
        } catch (...) {
            static int renderErrors = 0;
            if (renderErrors++ < 3) {
                HyprlandAPI::addNotification(PHANDLE, "Carousel rendering error", 
                                           CHyprColor{1.0, 0.2, 0.2, 1.0}, 1000);
            }
        }
    }
    
    void renderWorkspaceThumbnailProper(const WorkspaceThumbnail& thumbnail) {
        if (!g_pHyprOpenGL || !g_pHyprRenderer) return;
        
        // Following hyprexpo pattern: render workspace content to framebuffer first
        // then composite it to screen
        
        try {
            // Create a simple colored rectangle representing the workspace
            // (In a full implementation, this would render the actual workspace content)
            CHyprColor workspaceColor{0.2, 0.4, 0.8, thumbnail.alpha}; // Blue with transparency
            
            // Use official OpenGL API to render rectangle
            g_pHyprOpenGL->renderRect(thumbnail.box, workspaceColor);
            
            // Add workspace number/label
            if (thumbnail.workspace) {
                // TODO: In full implementation, render workspace content using g_pHyprRenderer
                // g_pHyprRenderer->renderWorkspace(pMonitor, thumbnail.workspace, &thumbnailBox);
            }
            
        } catch (...) {
            // Silent fallback for now
        }
    }
    
    void renderSelectionIndicatorProper(const WorkspaceThumbnail& thumbnail) {
        if (!g_pHyprOpenGL) return;
        
        try {
            // Render bright selection border (following hyprexpo pattern)
            CHyprColor highlightColor{1.0, 0.8, 0.2, 1.0}; // Bright yellow/orange
            
            // Create thick selection border
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
    
    void renderWorkspaceThumbnail(const WorkspaceThumbnail& thumbnail) {
        if (!g_pHyprOpenGL) return;
        
        // Render workspace content (works in demo mode too)
        renderWorkspaceStub(thumbnail.workspace, thumbnail.box, thumbnail.alpha);
    }
    
    void renderWorkspaceStub(SP<CWorkspace> workspace, const CBox& box, float alpha) {
        // Alternative rendering method when g_pHyprOpenGL is not available
        if (!g_pHyprOpenGL) {
            // Use notification-based visual feedback as fallback
            static int notificationCount = 0;
            static auto lastNotificationTime = std::chrono::steady_clock::now();
            auto now = std::chrono::steady_clock::now();
            
            // Only show notification every 2 seconds to avoid spam
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastNotificationTime).count() >= 2) {
                notificationCount++;
                std::string msg = "Carousel Visual " + std::to_string(notificationCount) + " - OpenGL not available";
                HyprlandAPI::addNotification(PHANDLE, msg, 
                                           CHyprColor{0.2, 0.4, 0.8, 1.0}, 1000);
                lastNotificationTime = now;
            }
            return;
        }
        
        // Normal OpenGL rendering (if available)
        CHyprColor boxColor{0.2, 0.4, 0.8, alpha}; // Blue with transparency
        CHyprColor borderColor{1.0, 1.0, 1.0, alpha}; // White border
        
        try {
            // Render main workspace box
            g_pHyprOpenGL->renderRect(box, boxColor);
            
            // Render border to make it more visible
            CBox borderBox = {box.x - 2, box.y - 2, box.width + 4, box.height + 4};
            g_pHyprOpenGL->renderRect(borderBox, borderColor);
        } catch (...) {
            static int renderAttempts = 0;
            if (renderAttempts++ < 5) {
                HyprlandAPI::addNotification(PHANDLE, "OpenGL render failed", 
                                           CHyprColor{1.0, 0.2, 0.2, 1.0}, 500);
            }
        }
    }
    
    void renderSelectionIndicator(const WorkspaceThumbnail& thumbnail) {
        if (!g_pHyprOpenGL) return;
        
        // Render a bright border around the selected workspace
        CHyprColor highlightColor{1.0, 0.8, 0.2, 1.0}; // Bright yellow/orange
        
        try {
            // Create thick selection border
            CBox borderBox = {
                thumbnail.box.x - 8, 
                thumbnail.box.y - 8, 
                thumbnail.box.width + 16, 
                thumbnail.box.height + 16
            };
            g_pHyprOpenGL->renderRect(borderBox, highlightColor);
        } catch (...) {
            // Fallback notification for selection
            static int selectionRenders = 0;
            if (selectionRenders++ < 3) {
                HyprlandAPI::addNotification(PHANDLE, "Selected workspace highlighted", 
                                           CHyprColor{1.0, 0.8, 0.2, 1.0}, 300);
            }
        }
    }
    
    void cleanup() {
        m_vThumbnails.clear();
        m_iSelectedWorkspace = 0;
        m_fRotationAngle = 0.0f;
    }
};

// Event callback functions following Hyprspace pattern
void onRenderCallback(void*, SCallbackInfo&, std::any) {
    CarouselPlugin::getInstance().onRender();
}

// Test function to check compositor state  
SDispatchResult carousel_test(std::string) {
    std::string status = "Compositor test: ";
    
    // Try alternative access methods
    try {
        // Method 1: Check if global pointers exist
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
        
        // Method 2: Try using HyprlandAPI to get version info (this should work)
        auto version = HyprlandAPI::getHyprlandVersion(PHANDLE);
        if (!version.hash.empty()) {
            status += "API=OK ";
        } else {
            status += "API=FAIL ";
        }
        
        // Method 3: Try to invoke a simple command to test if compositor responds
        auto result = HyprlandAPI::invokeHyprctlCommand("dispatch", "workspace 1");
        if (result.empty()) {
            status += "dispatch=FAIL";
        } else {
            status += "dispatch=OK";
        }
        
    } catch (...) {
        status += "EXCEPTION during test";
    }
    
    HyprlandAPI::addNotification(PHANDLE, status, 
                               CHyprColor{0.2, 0.8, 1.0, 1.0}, 4000);
    return SDispatchResult{};
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
    APICALL EXPORT std::string PLUGIN_API_VERSION() {
        return HYPRLAND_API_VERSION;
    }
    
    APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
        PHANDLE = handle;
        
        // Version check (critical for stability)
        const std::string HASH = __hyprland_api_get_hash();
        if (HASH != GIT_COMMIT_HASH) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Version mismatch!", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            throw std::runtime_error("[carousel] Version mismatch");
        }
        
        // Register configuration values first (optional - plugin works without these)
        try {
            HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:radius", 
                                       Hyprlang::CConfigValue((Hyprlang::INT)800));
            HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:spacing", 
                                       Hyprlang::CConfigValue((Hyprlang::FLOAT)1.2f));
            HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:transparency_gradient", 
                                       Hyprlang::CConfigValue((Hyprlang::FLOAT)0.3f));
            HyprlandAPI::addConfigValue(PHANDLE, "plugin:carousel:animation_duration", 
                                       Hyprlang::CConfigValue((Hyprlang::INT)300));
        } catch (...) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Config registration failed, using defaults", 
                                       CHyprColor{1.0, 1.0, 0.2, 1.0}, 2000);
        }
        
        // Initialize plugin
        auto& plugin = CarouselPlugin::getInstance();
        if (!plugin.initialize()) {
            HyprlandAPI::addNotification(PHANDLE, "[Carousel] Failed to initialize!", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            return {"Hypr Carousel", "Failed to initialize", "Claude", "1.0.0"};
        }
        
        // Register dispatchers using modern API
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:test", carousel_test);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:toggle", carousel_toggle);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:next", carousel_next);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:prev", carousel_prev);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:select", carousel_select);
        HyprlandAPI::addDispatcherV2(PHANDLE, "carousel:exit", carousel_exit);
        
        // Register event callbacks for 3D rendering
        static auto renderCallback = HyprlandAPI::registerCallbackDynamic(
            PHANDLE, "render", onRenderCallback);
        
        HyprlandAPI::addNotification(PHANDLE, "[Carousel] Plugin loaded successfully!", 
                                   CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);
        
        return {"Hypr Carousel", "3D workspace carousel following Hyprspace patterns", "Claude", "1.0.0"};
    }
    
    APICALL EXPORT void PLUGIN_EXIT() {
        // Clean exit
        CarouselPlugin::getInstance().exitCarousel();
    }
}