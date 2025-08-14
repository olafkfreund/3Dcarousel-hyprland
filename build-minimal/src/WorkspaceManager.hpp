#pragma once
#include <vector>

class WorkspaceManager {
public:
    WorkspaceManager();
    ~WorkspaceManager();
    
    bool initialize();
    void cleanup();
    
    void selectNext();
    void selectPrevious();
    void selectWorkspace(int index);
    
    int getCurrentSelection() const { return m_currentSelection; }
    int getWorkspaceCount() const { return m_workspaceCount; }
    
    void refreshWorkspaces();
    
private:
    int m_currentSelection = 0;
    int m_workspaceCount = 0;
};