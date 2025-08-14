#include "AnimationEngine.hpp"
#include <cmath>

AnimationEngine::AnimationEngine() {}

AnimationEngine::~AnimationEngine() {
    cleanup();
}

bool AnimationEngine::initialize() {
    return true;
}

void AnimationEngine::cleanup() {
    stopCarouselAnimation();
}

void AnimationEngine::startCarouselAnimation() {
    m_isAnimating = true;
    m_animationStart = std::chrono::steady_clock::now();
}

void AnimationEngine::stopCarouselAnimation() {
    m_isAnimating = false;
}

void AnimationEngine::startGrowAnimation() {
    m_animationStart = std::chrono::steady_clock::now();
}

void AnimationEngine::update() {
    if (!m_isAnimating) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_animationStart).count();
    
    if (elapsed >= m_animationDuration) {
        m_currentRotation = m_targetRotation;
        m_isAnimating = false;
        return;
    }
    
    float t = static_cast<float>(elapsed) / m_animationDuration;
    t = easeInOutCubic(t);
    
    m_currentRotation = m_currentRotation + (m_targetRotation - m_currentRotation) * t;
}

float AnimationEngine::easeInOutCubic(float t) {
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    } else {
        float p = 2.0f * t - 2.0f;
        return 1.0f + p * p * p / 2.0f;
    }
}