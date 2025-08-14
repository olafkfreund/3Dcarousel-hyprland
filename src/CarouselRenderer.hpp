#pragma once

// OpenGL and math libraries
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>
#include <cmath>

struct WorkspaceFrame {
    GLuint texture;
    GLuint framebuffer;
    int width, height;
    float angle;
    float distance;
    float alpha;
};

class CarouselRenderer {
public:
    CarouselRenderer();
    ~CarouselRenderer();
    
    bool initialize();
    void cleanup();
    
    void render();
    void updateWorkspacePositions(float rotation, int selectedIndex);
    void captureWorkspaces();
    
    void setCarouselRadius(float radius) { m_carouselRadius = radius; }
    void setWorkspaceSpacing(float spacing) { m_workspaceSpacing = spacing; }
    void setTransparencyGradient(float gradient) { m_transparencyGradient = gradient; }
    
private:
    bool createShaders();
    bool createGeometry();
    void renderWorkspace(const WorkspaceFrame& frame, const glm::mat4& mvp);
    
    GLuint m_shaderProgram;
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    
    GLuint m_VAO, m_VBO, m_EBO;
    
    // Shader uniforms
    GLint m_uMVP;
    GLint m_uTexture;
    GLint m_uAlpha;
    GLint m_uSelected;
    
    std::vector<WorkspaceFrame> m_workspaceFrames;
    
    float m_carouselRadius = 800.0f;
    float m_workspaceSpacing = 1.2f;
    float m_transparencyGradient = 0.3f;
    
    int m_selectedWorkspace = 0;
    float m_currentRotation = 0.0f;
};