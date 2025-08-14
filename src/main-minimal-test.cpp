// Minimal test plugin to debug initialization issues
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/helpers/Color.hpp>

// Global plugin handle
inline HANDLE PHANDLE = nullptr;

// Minimal test functions
SDispatchResult test_toggle(std::string) {
    HyprlandAPI::addNotification(PHANDLE, "Test toggle worked!", 
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 2000);
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
            HyprlandAPI::addNotification(PHANDLE, "Version mismatch!", 
                                       CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            throw std::runtime_error("Version mismatch");
        }
        
        // Simple initialization - no complex setup
        HyprlandAPI::addNotification(PHANDLE, "Minimal plugin initializing...", 
                                   CHyprColor{0.2, 0.2, 1.0, 1.0}, 3000);
        
        // Register one simple dispatcher
        HyprlandAPI::addDispatcherV2(PHANDLE, "test:toggle", test_toggle);
        
        HyprlandAPI::addNotification(PHANDLE, "Minimal plugin loaded successfully!", 
                                   CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);
        
        return {"Test Plugin", "Minimal test for debugging", "Claude", "1.0.0"};
    }
    
    APICALL EXPORT void PLUGIN_EXIT() {
        // Clean exit
    }
}