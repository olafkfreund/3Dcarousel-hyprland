#include "CarouselRenderer.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uMVP;
out vec2 TexCoord;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform float uAlpha;
uniform bool uSelected;

void main() {
    vec4 texColor = texture(uTexture, TexCoord);
    
    if (uSelected) {
        texColor.rgb += 0.2;
    }
    
    FragColor = vec4(texColor.rgb, texColor.a * uAlpha);
}
)";

CarouselRenderer::CarouselRenderer() {}

CarouselRenderer::~CarouselRenderer() {
    cleanup();
}

bool CarouselRenderer::initialize() {
    if (!createShaders()) {
        std::cerr << "Failed to create shaders" << std::endl;
        return false;
    }
    
    if (!createGeometry()) {
        std::cerr << "Failed to create geometry" << std::endl;
        return false;
    }
    
    return true;
}

bool CarouselRenderer::createShaders() {
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(m_vertexShader);
    
    GLint success;
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(m_vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(m_fragmentShader);
    
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(m_fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return false;
    }
    
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);
    
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }
    
    m_uMVP = glGetUniformLocation(m_shaderProgram, "uMVP");
    m_uTexture = glGetUniformLocation(m_shaderProgram, "uTexture");
    m_uAlpha = glGetUniformLocation(m_shaderProgram, "uAlpha");
    m_uSelected = glGetUniformLocation(m_shaderProgram, "uSelected");
    
    return true;
}

bool CarouselRenderer::createGeometry() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    return true;
}

void CarouselRenderer::render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    
    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_VAO);
    
    auto monitor = g_pCompositor->m_pLastMonitor;
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 
        (float)monitor->vecSize.x / (float)monitor->vecSize.y, 
        0.1f, 
        2000.0f
    );
    
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 1000.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    for (size_t i = 0; i < m_workspaceFrames.size(); ++i) {
        const auto& frame = m_workspaceFrames[i];
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, frame.angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -frame.distance));
        model = glm::scale(model, glm::vec3(400.0f, 300.0f, 1.0f));
        
        glm::mat4 mvp = projection * view * model;
        
        glUniformMatrix4fv(m_uMVP, 1, GL_FALSE, &mvp[0][0]);
        glUniform1f(m_uAlpha, frame.alpha);
        glUniform1i(m_uSelected, i == m_selectedWorkspace);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frame.texture);
        glUniform1i(m_uTexture, 0);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void CarouselRenderer::updateWorkspacePositions(float rotation, int selectedIndex) {
    m_currentRotation = rotation;
    m_selectedWorkspace = selectedIndex;
    
    for (size_t i = 0; i < m_workspaceFrames.size(); ++i) {
        auto& frame = m_workspaceFrames[i];
        
        float angleOffset = (float)i * m_workspaceSpacing * (2.0f * M_PI / m_workspaceFrames.size());
        frame.angle = rotation + angleOffset;
        
        if ((int)i == selectedIndex) {
            frame.distance = m_carouselRadius * 0.7f;
            frame.alpha = 1.0f;
        } else {
            frame.distance = m_carouselRadius;
            float angleDiff = std::abs(frame.angle - (rotation + selectedIndex * angleOffset));
            angleDiff = std::min(angleDiff, (float)(2.0f * M_PI - angleDiff));
            frame.alpha = std::max(0.3f, 1.0f - angleDiff * m_transparencyGradient);
        }
    }
}

void CarouselRenderer::captureWorkspaces() {
    auto workspaces = g_pCompositor->m_vWorkspaces;
    
    m_workspaceFrames.clear();
    m_workspaceFrames.reserve(workspaces.size());
    
    for (auto& workspace : workspaces) {
        if (!workspace->m_bIsSpecialWorkspace && workspace->m_windows.size() > 0) {
            WorkspaceFrame frame;
            frame.width = 1920;
            frame.height = 1080;
            frame.angle = 0.0f;
            frame.distance = m_carouselRadius;
            frame.alpha = 0.7f;
            
            m_workspaceFrames.push_back(frame);
        }
    }
}

void CarouselRenderer::cleanup() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
    if (m_vertexShader) glDeleteShader(m_vertexShader);
    if (m_fragmentShader) glDeleteShader(m_fragmentShader);
    
    for (auto& frame : m_workspaceFrames) {
        if (frame.texture) glDeleteTextures(1, &frame.texture);
        if (frame.framebuffer) glDeleteFramebuffers(1, &frame.framebuffer);
    }
}