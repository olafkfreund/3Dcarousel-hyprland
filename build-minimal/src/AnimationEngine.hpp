#pragma once
#include <chrono>

class AnimationEngine {
public:
    AnimationEngine();
    ~AnimationEngine();
    
    bool initialize();
    void cleanup();
    
    void startCarouselAnimation();
    void stopCarouselAnimation();
    void startGrowAnimation();
    
    void update();
    
    float getCurrentRotation() const { return m_currentRotation; }
    float getGrowScale() const { return m_growScale; }
    
    bool isAnimating() const { return m_isAnimating; }
    
    void setAnimationDuration(int durationMs) { m_animationDuration = durationMs; }
    
private:
    bool m_isAnimating = false;
    float m_currentRotation = 0.0f;
    float m_targetRotation = 0.0f;
    float m_growScale = 1.0f;
    
    int m_animationDuration = 300;
    std::chrono::steady_clock::time_point m_animationStart;
    
    float easeInOutCubic(float t);
};