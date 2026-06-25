#include "Viewport.h"
#include "../utils/ImID.h"
#include <imgui.h>
#include <algorithm>

namespace NexusSDK {
namespace UI {

std::shared_ptr<Viewport> Viewport::s_instance = nullptr;

Viewport::Viewport() : Container() {
    m_id = ImID::Create("Viewport");
    m_position = ImVec2(0, 0);
}

void Viewport::Initialize() {
    if (!s_instance) {
        s_instance = std::make_shared<Viewport>();
    }
}

void Viewport::Shutdown() {
    s_instance.reset();
}

std::shared_ptr<Viewport> Viewport::Get() {
    return s_instance;
}

void Viewport::Render(float uiScale) {
    if (!s_instance) return;

    if (s_instance->m_currentScale != uiScale) {
        s_instance->m_currentScale = uiScale;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(displaySize);

    ImGuiWindowFlags flags = 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoFocusOnAppearing | 
        ImGuiWindowFlags_NoBringToFrontOnFocus | 
        ImGuiWindowFlags_NoBackground;

    flags |= ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin(s_instance->GetID().c_str(), nullptr, flags)) {
        Rectangle clientBounds;
        clientBounds.X = 0;
        clientBounds.Y = 0;
        clientBounds.Width = displaySize.x;
        clientBounds.Height = displaySize.y;

        // Iterate a copy to avoid iterator invalidation if a child removes itself
        auto children = s_instance->GetChildren();
        for (auto& child : children) {
            child->Draw(clientBounds, uiScale);
        }
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
}

} // namespace UI
} // namespace NexusSDK
