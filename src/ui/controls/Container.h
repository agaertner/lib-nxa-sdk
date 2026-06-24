#pragma once

#include "ControlBase.h"
#include <nexus-imgui/imgui.h>

namespace NexusSDK {
namespace UI {

class Container : public ControlBase {
public:
    Container() : ControlBase() {
        InputCapture = CaptureType::None;
    }
    virtual ~Container() = default;

    // Optional padding/margin logic could go here in the future
    
protected:
    virtual void OnDraw(const Rectangle& bounds, float scale) override {
        ImVec2 drawPos = bounds.GetMin();
        ImVec2 drawSize = ImVec2(bounds.Width, bounds.Height);

        if (m_size.x != 0.0f) drawSize.x = m_size.x * scale;
        if (m_size.y != 0.0f) drawSize.y = m_size.y * scale;

        // ImGui::BeginChild creates an isolated, scrollable region with its own clipping rect.
        ImGui::SetCursorScreenPos(drawPos);
        
        bool isVisible = ImGui::BeginChild(m_id.c_str(), drawSize, false, ImGuiWindowFlags_NoBackground);
        
        if (isVisible) {
            ImVec2 scrolledPos = ImGui::GetCursorScreenPos();
            Rectangle clientBounds;
            clientBounds.X = scrolledPos.x;
            clientBounds.Y = scrolledPos.y;
            clientBounds.Width = drawSize.x;
            clientBounds.Height = drawSize.y;

            DrawChildren(clientBounds, scale);
        }
        
        ImGui::EndChild();
    }
};

} // namespace UI
} // namespace NexusSDK
