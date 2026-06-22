#pragma once

#include "Container.h"
#include "../../utils/AsyncTexture.h"
#include <imgui/imgui.h>
#include <functional>
#include <algorithm>

namespace NexusSDK {
namespace UI {

class WindowBase : public Container {
public:
    WindowBase() : Container() {}
    virtual ~WindowBase() = default;

    std::string Title = "";
    
    // Background Texture
    std::shared_ptr<AsyncTexture> BackgroundTexture;
    
    // Titlebar Textures
    std::shared_ptr<AsyncTexture> TitlebarActiveTexture;
    std::shared_ptr<AsyncTexture> TitlebarInactiveTexture;

    // Custom Resize Grip Textures
    std::shared_ptr<AsyncTexture> ResizeHoverTexture;
    std::shared_ptr<AsyncTexture> ResizeActiveTexture;
    std::shared_ptr<AsyncTexture> ResizeInactiveTexture;

    bool IsResizable = true;
    bool IsOpen = true;
    float TitlebarHeight = 40.0f;

    // Callbacks
    std::function<void()> OnClose;

protected:
    virtual void OnRender() override {
        if (!IsOpen) return;

        // We disable the native title bar to draw our own. 
        // We disable native resize to use our custom bottom-right corner textures.
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        
        // Disable native background if a texture is provided
        if (BackgroundTexture && BackgroundTexture->Get()) {
            flags |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::SetNextWindowSize(m_size, ImGuiCond_FirstUseEver);

        if (ImGui::Begin(m_id.c_str(), &IsOpen, flags)) {
            m_size = ImGui::GetWindowSize();
            m_position = ImGui::GetWindowPos();
            
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // 1. Draw Custom Background
            if (BackgroundTexture && BackgroundTexture->Get()) {
                drawList->AddImage(
                    (ImTextureID)BackgroundTexture->Get(),
                    m_position,
                    ImVec2(m_position.x + m_size.x, m_position.y + m_size.y),
                    ImVec2(0,0), ImVec2(1,1), ImGui::GetColorU32(IM_COL32_WHITE)
                );
            }

            // 2. Draw Custom Titlebar
            bool isFocused = ImGui::IsWindowFocused();
            std::shared_ptr<AsyncTexture> tbTex = isFocused ? TitlebarActiveTexture : TitlebarInactiveTexture;
            
            if (tbTex && tbTex->Get()) {
                drawList->AddImage(
                    (ImTextureID)tbTex->Get(),
                    m_position,
                    ImVec2(m_position.x + m_size.x, m_position.y + TitlebarHeight),
                    ImVec2(0,0), ImVec2(1,1), ImGui::GetColorU32(IM_COL32_WHITE)
                );
            } else {
                // Native fallback for titlebar: Draw a rect
                drawList->AddRectFilled(
                    m_position, 
                    ImVec2(m_position.x + m_size.x, m_position.y + TitlebarHeight), 
                    ImGui::GetColorU32(isFocused ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBg)
                );
            }

            // 3. Draw Title Text
            if (!Title.empty()) {
                drawList->AddText(ImVec2(m_position.x + 15.0f, m_position.y + 10.0f), ImColor(255, 255, 255), Title.c_str());
            }

            // 4. Custom Dragging Area (Titlebar)
            ImGui::SetCursorPos(ImVec2(0, 0));
            ImGui::InvisibleButton("##titleBarDrag", ImVec2(m_size.x, TitlebarHeight));
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                ImGui::SetWindowPos(ImVec2(m_position.x + delta.x, m_position.y + delta.y));
            }

            // 5. Content Container
            // We use a sub-child to trap children safely below the titlebar and prevent overlap
            ImGui::SetCursorPos(ImVec2(0, TitlebarHeight));
            if (ImGui::BeginChild("##windowContent", ImVec2(m_size.x, m_size.y - TitlebarHeight), false, ImGuiWindowFlags_NoBackground)) {
                RenderChildren();
            }
            ImGui::EndChild();

            // 6. Custom Resize Grip (Bottom Right)
            if (IsResizable) {
                float gripSize = 25.0f;
                ImVec2 gripLocalPos = ImVec2(m_size.x - gripSize, m_size.y - gripSize);
                ImGui::SetCursorPos(gripLocalPos);
                
                ImGui::InvisibleButton("##resizeGrip", ImVec2(gripSize, gripSize));
                bool isHovered = ImGui::IsItemHovered();
                bool isActive = ImGui::IsItemActive();

                std::shared_ptr<AsyncTexture> resizeTex = ResizeInactiveTexture;
                if (isActive) resizeTex = ResizeActiveTexture;
                else if (isHovered) resizeTex = ResizeHoverTexture;

                if (resizeTex && resizeTex->Get()) {
                    drawList->AddImage(
                        (ImTextureID)resizeTex->Get(),
                        ImVec2(m_position.x + gripLocalPos.x, m_position.y + gripLocalPos.y),
                        ImVec2(m_position.x + m_size.x, m_position.y + m_size.y),
                        ImVec2(0,0), ImVec2(1,1), ImGui::GetColorU32(IM_COL32_WHITE)
                    );
                } else {
                    // Fallback to a simple drawn triangle
                    ImU32 gripColor = ImGui::GetColorU32(isActive ? ImGuiCol_ResizeGripActive : (isHovered ? ImGuiCol_ResizeGripHovered : ImGuiCol_ResizeGrip));
                    drawList->PathLineTo(ImVec2(m_position.x + m_size.x - gripSize, m_position.y + m_size.y));
                    drawList->PathLineTo(ImVec2(m_position.x + m_size.x, m_position.y + m_size.y));
                    drawList->PathLineTo(ImVec2(m_position.x + m_size.x, m_position.y + m_size.y - gripSize));
                    drawList->PathFillConvex(gripColor);
                }

                // Handle manual resizing
                if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    ImVec2 delta = ImGui::GetIO().MouseDelta;
                    ImVec2 newSize = ImVec2((std::max)(100.0f, m_size.x + delta.x), (std::max)(100.0f, m_size.y + delta.y));
                    ImGui::SetWindowSize(newSize);
                }
            }
        }
        ImGui::End();

        if (!IsOpen && OnClose) {
            OnClose();
        }
    }
};

} // namespace UI
} // namespace NexusSDK
