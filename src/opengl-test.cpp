// Minimal OpenGL Test Plugin - Debug timing and availability
// Tests when g_pHyprOpenGL becomes available during Hyprland startup

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/render/OpenGL.hpp>

#include <chrono>
#include <thread>

// Global plugin handle
inline HANDLE PHANDLE = nullptr;

// Test state tracking
struct OpenGLTestState {
    bool hasTestedInit = false;
    bool hasTestedRender = false;
    int renderCallCount = 0;
    std::chrono::steady_clock::time_point pluginStartTime;
    
    OpenGLTestState() : pluginStartTime(std::chrono::steady_clock::now()) {}
};

static OpenGLTestState g_testState;

// Test function to check OpenGL availability at different times
void testOpenGLAvailability(const std::string& context) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_testState.pluginStartTime);
    
    std::string msg = "[" + std::to_string(elapsed.count()) + "ms] " + context + ": ";
    
    // Test all relevant globals
    if (!g_pCompositor) {
        msg += "g_pCompositor=NULL ";
    } else {
        msg += "g_pCompositor=OK ";
    }
    
    if (!g_pHyprRenderer) {
        msg += "g_pHyprRenderer=NULL ";
    } else {
        msg += "g_pHyprRenderer=OK ";
    }
    
    if (!g_pHyprOpenGL) {
        msg += "g_pHyprOpenGL=NULL ";
    } else {
        msg += "g_pHyprOpenGL=OK ";
        
        // Try a simple OpenGL operation
        try {
            // Test basic rect rendering
            CBox testBox{100, 100, 200, 100};
            CHyprColor testColor{1.0, 0.0, 0.0, 0.8}; // Red
            g_pHyprOpenGL->renderRect(testBox, testColor);
            msg += "RENDER=OK ";
        } catch (...) {
            msg += "RENDER=FAIL ";
        }
    }
    
    HyprlandAPI::addNotification(PHANDLE, msg, CHyprColor{0.2, 0.8, 1.0, 1.0}, 3000);
}

// Render callback to test during render phase
void onRenderCallback(void*, SCallbackInfo&, std::any) {
    g_testState.renderCallCount++;
    
    // Test OpenGL availability during first few render calls
    if (g_testState.renderCallCount <= 5) {
        testOpenGLAvailability("Render#" + std::to_string(g_testState.renderCallCount));
    }
    
    // Try actual rendering if OpenGL is available
    if (g_pHyprOpenGL && g_testState.renderCallCount <= 3) {
        try {
            // Simple test rectangle
            CBox testBox{50 + g_testState.renderCallCount * 60, 50, 50, 50};
            CHyprColor testColor{0.0, 1.0, 0.0, 0.9}; // Green
            g_pHyprOpenGL->renderRect(testBox, testColor);
        } catch (...) {
            HyprlandAPI::addNotification(PHANDLE, "Render attempt failed", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 1000);
        }
    }
}

// Delayed test function
SDispatchResult delayedOpenGLTest(std::string) {
    std::thread([]{
        // Test at various delays to see when OpenGL becomes available
        std::vector<int> delays = {100, 500, 1000, 2000, 5000}; // milliseconds
        
        for (int delay : delays) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            testOpenGLAvailability("Delayed+" + std::to_string(delay) + "ms");
        }
    }).detach();
    
    return SDispatchResult{};
}

// Manual test function
SDispatchResult manualOpenGLTest(std::string) {
    testOpenGLAvailability("Manual");
    return SDispatchResult{};
}

// Plugin entry points
extern "C" {
    APICALL EXPORT std::string PLUGIN_API_VERSION() {
        return HYPRLAND_API_VERSION;
    }
    
    APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
        PHANDLE = handle;
        
        // Test OpenGL availability immediately during plugin init
        testOpenGLAvailability("Init");
        
        // Register dispatchers for manual testing
        HyprlandAPI::addDispatcherV2(PHANDLE, "opengl:test", manualOpenGLTest);
        HyprlandAPI::addDispatcherV2(PHANDLE, "opengl:delayed", delayedOpenGLTest);
        
        // Register render callback to test during render phase
        static auto renderCallback = HyprlandAPI::registerCallbackDynamic(
            PHANDLE, "render", onRenderCallback);
        
        // Test after a short delay to see if globals become available
        std::thread([]{
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            testOpenGLAvailability("PostInit+100ms");
        }).detach();
        
        HyprlandAPI::addNotification(PHANDLE, "[OpenGL Test] Plugin loaded - testing availability", 
                                   CHyprColor{0.2, 1.0, 0.2, 1.0}, 2000);
        
        return {"OpenGL Test", "Debug OpenGL availability and timing", "Claude", "1.0.0"};
    }
    
    APICALL EXPORT void PLUGIN_EXIT() {
        // Clean exit
    }
}