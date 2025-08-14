// Minimal test plugin for Hyprland 0.50.0
// This verifies basic plugin loading before adding complex functionality

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprgraphics/color/Color.hpp>

// Global plugin handle (required by Hyprland plugin system)
inline HANDLE PHANDLE = nullptr;

extern "C" {
    APICALL EXPORT const char* pluginAPIVersion() {
        return HYPRLAND_API_VERSION;
    }

    APICALL EXPORT PLUGIN_DESCRIPTION_INFO pluginInit(HANDLE handle) {
        PHANDLE = handle;
        
        // Version check (critical for stability)
        const std::string HASH = __hyprland_api_get_hash();
        if (HASH != GIT_COMMIT_HASH) {
            HyprlandAPI::addNotification(PHANDLE, "[Test] Version mismatch!", 
                                       Hyprgraphics::CColor{1.0, 0.2, 0.2, 1.0}, 5000);
            throw std::runtime_error("API version mismatch");
        }
        
        // Start with minimal functionality
        HyprlandAPI::addNotification(PHANDLE, "[Test] Minimal plugin loaded!", 
                                     Hyprgraphics::CColor{0.2, 1.0, 0.2, 1.0}, 3000);
        
        // Add a simple test dispatcher
        HyprlandAPI::addDispatcher(PHANDLE, "test:hello", [](std::string arg) {
            HyprlandAPI::addNotification(PHANDLE, "[Test] Hello World!", 
                                       Hyprgraphics::CColor{0.2, 0.8, 1.0, 1.0}, 2000);
        });
        
        return {"Test Plugin", "Minimal test for loading verification", "Claude", "1.0.0"};
    }

    APICALL EXPORT void pluginExit() {
        // Clean exit - nothing to cleanup in minimal version
        if (PHANDLE) {
            HyprlandAPI::addNotification(PHANDLE, "[Test] Plugin unloading...", 
                                       Hyprgraphics::CColor{1.0, 1.0, 0.2, 1.0}, 2000);
        }
    }
}