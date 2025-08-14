#include "WorkspaceManager.hpp"
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Workspace.hpp>
#include <algorithm>

WorkspaceManager::WorkspaceManager() {}

WorkspaceManager::~WorkspaceManager() {
    cleanup();
}

bool WorkspaceManager::initialize() {
    refreshWorkspaces();
    return true;
}

void WorkspaceManager::cleanup() {
}

void WorkspaceManager::selectNext() {
    if (m_workspaceCount > 0) {
        m_currentSelection = (m_currentSelection + 1) % m_workspaceCount;
    }
}

void WorkspaceManager::selectPrevious() {
    if (m_workspaceCount > 0) {
        m_currentSelection = (m_currentSelection - 1 + m_workspaceCount) % m_workspaceCount;
    }
}

void WorkspaceManager::selectWorkspace(int index) {
    if (index >= 0 && index < m_workspaceCount) {
        m_currentSelection = index;
    }
}

void WorkspaceManager::refreshWorkspaces() {
    auto workspaces = g_pCompositor->m_vWorkspaces;
    
    m_workspaceCount = 0;
    for (auto& workspace : workspaces) {
        if (!workspace->m_bIsSpecialWorkspace && workspace->m_windows.size() > 0) {
            m_workspaceCount++;
        }
    }
    
    m_currentSelection = std::min(m_currentSelection, m_workspaceCount - 1);
    if (m_currentSelection < 0) m_currentSelection = 0;
}