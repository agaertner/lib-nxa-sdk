#pragma once

#include "Container.h"
#include <imgui.h>

namespace NexusSDK {
namespace UI {

class Panel : public Container {
public:
    Panel() : Container() {}
    virtual ~Panel() = default;

    bool CanScroll = true;

protected:
    virtual void OnDraw(const Rectangle& bounds) override {
        float scale = UIScale::Get();
        ImVec2 drawSize = ImVec2(bounds.Width, bounds.Height);

        if (m_size.x != 0.0f) drawSize.x = m_size.x * scale;
        if (m_size.y != 0.0f) drawSize.y = m_size.y * scale;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground;
        if (!CanScroll) {
            flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        }

        // ImGui::BeginChild creates an isolated region with its own clipping rect.
        bool isVisible = ImGui::BeginChild(m_id.c_str(), drawSize, false, flags);
        
        if (isVisible) {
            ImVec2 scrolledPos = ImGui::GetCursorScreenPos();
            Rectangle clientBounds;
            clientBounds.X = scrolledPos.x;
            clientBounds.Y = scrolledPos.y;
            clientBounds.Width = drawSize.x;
            clientBounds.Height = drawSize.y;

            DrawChildren(clientBounds);
        }
        
        ImGui::EndChild();
    }
};

} // namespace UI
} // namespace NexusSDK
