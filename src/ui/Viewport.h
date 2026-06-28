#pragma once

#include "controls/Container.h"
#include "../utils/ImID.h"
#include "../utils/ImStateGuards.h"
#include <memory>
#include <string>
#include <imgui.h>

namespace NexusSDK {
namespace UI {

class Viewport : public Container {
public:
    Viewport() : Container() {
        m_id = ImID::Create("Viewport");
        m_position = ImVec2(0, 0);
    }
    ~Viewport() = default;

    // Singleton access
    static void Initialize() {
        if (!s_instance) {
            s_instance = std::make_shared<Viewport>();
        }
    }

    static void Shutdown() {
        s_instance.reset();
    }

    static std::shared_ptr<Viewport> Get() {
        return s_instance;
    }

    // The main render hook to be called every frame
    static void Render(float uiScale = 1.0f) {
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
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoInputs;

        StyleGuard padding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        StyleGuard border(ImGuiStyleVar_WindowBorderSize, 0.0f);

        if (ImGui::Begin(s_instance->GetID().c_str(), nullptr, flags)) {
            Rectangle clientBounds;
            clientBounds.X = 0;
            clientBounds.Y = 0;
            clientBounds.Width = displaySize.x;
            clientBounds.Height = displaySize.y;

            s_instance->Draw(clientBounds);
        }
        ImGui::End();
    }

    float GetCurrentScale() const { return m_currentScale; }

private:
    inline static std::shared_ptr<Viewport> s_instance = nullptr;
    float m_currentScale = 1.0f;
};

} // namespace UI
} // namespace NexusSDK
