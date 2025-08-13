#pragma once

// Fallback definitions for when Hyprland headers are not available
// This allows basic compilation testing

#ifndef HYPRLAND_FALLBACK_GUARD
#define HYPRLAND_FALLBACK_GUARD

#include <string>
#include <vector>
#include <memory>
#include <functional>

// Basic type definitions
struct CColor {
    float r, g, b, a;
    CColor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
};

struct Vector2D {
    double x, y;
};

// Mock workspace structure
struct CWorkspace {
    bool m_bIsSpecialWorkspace = false;
    std::vector<void*> m_windows;
};

// Mock compositor
struct CCompositor {
    std::vector<std::shared_ptr<CWorkspace>> m_vWorkspaces;
    struct {
        Vector2D vecSize{1920, 1080};
    }* m_pLastMonitor = nullptr;
};

// Global variables (mocked)
extern CCompositor* g_pCompositor;
extern void* PHANDLE;

// OpenGL constants
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_FLOAT 0x1406
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_BLEND 0x0BE2
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DEPTH_TEST 0x0B71
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_2D 0x0DE1
#define GL_TRIANGLES 0x0004
#define GL_UNSIGNED_INT 0x1405

// Mock OpenGL functions
inline unsigned int glCreateShader(int type) { return 1; }
inline void glShaderSource(int shader, int count, const char** string, const int* length) {}
inline void glCompileShader(int shader) {}
inline void glGetShaderiv(int shader, int pname, int* params) { *params = 1; }
inline void glGetShaderInfoLog(int shader, int bufSize, int* length, char* infoLog) { infoLog[0] = '\0'; }
inline unsigned int glCreateProgram() { return 1; }
inline void glAttachShader(int program, int shader) {}
inline void glLinkProgram(int program) {}
inline void glGetProgramiv(int program, int pname, int* params) { *params = 1; }
inline void glGetProgramInfoLog(int program, int bufSize, int* length, char* infoLog) { infoLog[0] = '\0'; }
inline int glGetUniformLocation(int program, const char* name) { return 1; }
inline void glGenVertexArrays(int n, unsigned int* arrays) { *arrays = 1; }
inline void glGenBuffers(int n, unsigned int* buffers) { *buffers = 1; }
inline void glBindVertexArray(unsigned int array) {}
inline void glBindBuffer(int target, unsigned int buffer) {}
inline void glBufferData(int target, long size, const void* data, int usage) {}
inline void glVertexAttribPointer(int index, int size, int type, bool normalized, int stride, const void* pointer) {}
inline void glEnableVertexAttribArray(int index) {}
inline void glEnable(int cap) {}
inline void glBlendFunc(int sfactor, int dfactor) {}
inline void glUseProgram(int program) {}
inline void glUniformMatrix4fv(int location, int count, bool transpose, const float* value) {}
inline void glUniform1f(int location, float v0) {}
inline void glUniform1i(int location, int v0) {}
inline void glActiveTexture(int texture) {}
inline void glBindTexture(int target, unsigned int texture) {}
inline void glDrawElements(int mode, int count, int type, const void* indices) {}
inline void glDisable(int cap) {}
inline void glDeleteVertexArrays(int n, const unsigned int* arrays) {}
inline void glDeleteBuffers(int n, const unsigned int* buffers) {}
inline void glDeleteProgram(unsigned int program) {}
inline void glDeleteShader(unsigned int shader) {}
inline void glDeleteTextures(int n, const unsigned int* textures) {}
inline void glDeleteFramebuffers(int n, const unsigned int* framebuffers) {}

// Plugin API definitions
#define APICALL
#define HANDLE void*

struct PluginDescInfo {
    const char* name;
    const char* description; 
    const char* version;
    const char* author;
};

#define PLUGIN_DESCRIPTION_INFO PluginDescInfo

// Mock API functions
namespace HyprlandAPI {
    inline void addNotification(void* handle, const std::string& text, const CColor& color, int timeout) {}
    inline void addDispatcher(void* handle, const std::string& name, std::function<void(std::string)> func) {}
}

// Constants
#define HYPRLAND_API_VERSION "1.0"

#endif // HYPRLAND_FALLBACK_GUARD