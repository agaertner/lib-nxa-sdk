#pragma once

#include "Container.h"

namespace NexusSDK {
namespace UI {

enum class FlowDirection {
    TopToBottom,
    LeftToRight
};

class FlowPanel : public Container {
public:
    FlowPanel() : Container() {}
    virtual ~FlowPanel() = default;

    FlowDirection ControlFlowDirection = FlowDirection::TopToBottom;
    float ControlPadding = 8.0f;

protected:
    virtual void OnRender() override {
        bool isVisible = ImGui::BeginChild(m_id.c_str(), m_size, false, ImGuiWindowFlags_NoBackground);
        
        if (isVisible) {
            for (size_t i = 0; i < m_children.size(); ++i) {
                m_children[i]->Render();
                
                if (i < m_children.size() - 1) {
                    if (ControlFlowDirection == FlowDirection::TopToBottom) {
                        ImGui::Dummy(ImVec2(0.0f, ControlPadding));
                    } else if (ControlFlowDirection == FlowDirection::LeftToRight) {
                        ImGui::SameLine(0.0f, ControlPadding);
                    }
                }
            }
        }
        
        ImGui::EndChild();
    }
};

} // namespace UI
} // namespace NexusSDK
