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
    virtual void OnDraw(const Rectangle& bounds, float scale) override {
        ImGui::SetCursorScreenPos(bounds.GetMin());
        bool isVisible = ImGui::BeginChild(m_id.c_str(), ImVec2(bounds.Width, bounds.Height), false, ImGuiWindowFlags_NoBackground);
        
        if (isVisible) {
            ImVec2 scrolledPos = ImGui::GetCursorScreenPos();
            Rectangle clientBounds = bounds;
            clientBounds.X = scrolledPos.x;
            clientBounds.Y = scrolledPos.y;

            float currentX = clientBounds.X;
            float currentY = clientBounds.Y;

            for (size_t i = 0; i < m_children.size(); ++i) {
                auto& child = m_children[i];
                if (!child->IsVisible()) continue;
                
                Rectangle childBounds;
                childBounds.X = currentX + (child->GetPosition().x * scale);
                childBounds.Y = currentY + (child->GetPosition().y * scale);
                childBounds.Width = child->GetSize(scale).x * scale;
                childBounds.Height = child->GetSize(scale).y * scale;

                child->Draw(childBounds, scale);
                
                if (ControlFlowDirection == FlowDirection::TopToBottom) {
                    currentY += childBounds.Height + (ControlPadding * scale);
                } else if (ControlFlowDirection == FlowDirection::LeftToRight) {
                    currentX += childBounds.Width + (ControlPadding * scale);
                }
            }
        }
        
        ImGui::EndChild();
    }
};

} // namespace UI
} // namespace NexusSDK
