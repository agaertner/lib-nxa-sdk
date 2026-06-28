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
    virtual ImVec2 GetAutoSize() const override {
        float totalWidth = 0.0f;
        float totalHeight = 0.0f;
        float maxWidth = 0.0f;
        float maxHeight = 0.0f;
        
        bool first = true;
        for (const auto& child : m_children) {
            if (!child->IsVisible()) continue;
            ImVec2 childSize = child->GetSize();
            
            if (ControlFlowDirection == FlowDirection::TopToBottom) {
                totalHeight += childSize.y + (first ? 0.0f : ControlPadding);
                if (childSize.x > maxWidth) maxWidth = childSize.x;
            } else {
                totalWidth += childSize.x + (first ? 0.0f : ControlPadding);
                if (childSize.y > maxHeight) maxHeight = childSize.y;
            }
            first = false;
        }
        
        float finalW = 0.0f;
        float finalH = 0.0f;
        if (ControlFlowDirection == FlowDirection::TopToBottom) {
            finalW = maxWidth;
            finalH = totalHeight;
        } else {
            finalW = totalWidth;
            finalH = maxHeight;
        }
        
        return ImVec2(
            (std::max)(finalW, m_size.x > 0.0f ? m_size.x : 0.0f),
            (std::max)(finalH, m_size.y > 0.0f ? m_size.y : 0.0f)
        );
    }

    virtual void OnDraw(const Rectangle& bounds) override {
        float scale = UIScale::Get();
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
                childBounds.X = currentX + (child->Left() * scale);
                childBounds.Y = currentY + (child->Top() * scale);
                childBounds.Width = child->GetSize().x * scale;
                childBounds.Height = child->GetSize().y * scale;

                child->Draw(childBounds);
                
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
