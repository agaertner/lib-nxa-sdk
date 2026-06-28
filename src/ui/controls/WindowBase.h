#pragma once

#include "Container.h"
#include "../../utils/AsyncTexture.h"
#include "../SpriteBatch.h"
#include <imgui.h>
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
    virtual void OnDraw(const Rectangle& bounds) override {
        if (!IsOpen) return;
        float scale = UIScale::Get();

        // We disable the native title bar to draw our own. 
        // We disable native resize to use our custom bottom-right corner textures.
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        
        // Disable native background if a texture is provided
        if (BackgroundTexture && BackgroundTexture->Get()) {
            flags |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::SetNextWindowPos(bounds.GetMin(), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(bounds.Width, bounds.Height), ImGuiCond_FirstUseEver);

        if (ImGui::Begin(m_id.c_str(), &IsOpen, flags)) {
            ImVec2 scaledSize = ImGui::GetWindowSize();
            ImVec2 scaledPos = ImGui::GetWindowPos();
            
            m_size.x = scaledSize.x / scale;
            m_size.y = scaledSize.y / scale;
            m_position.x = scaledPos.x / scale;
            m_position.y = scaledPos.y / scale;

            // 1. Draw Custom Background
            if (BackgroundTexture && BackgroundTexture->Get()) {
                SpriteBatch::DrawTexture(
                    BackgroundTexture.get(),
                    scaledPos,
                    ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y)
                );
            }

            float titlebarHeightScaled = TitlebarHeight * scale;

            // 2. Draw Custom Titlebar
            bool isFocused = ImGui::IsWindowFocused();
            std::shared_ptr<AsyncTexture> tbTex = isFocused ? TitlebarActiveTexture : TitlebarInactiveTexture;
            
            if (tbTex && tbTex->Get()) {
                SpriteBatch::DrawTexture(
                    tbTex.get(),
                    scaledPos,
                    ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + titlebarHeightScaled)
                );
            } else {
                // Native fallback for titlebar: Draw a rect
                SpriteBatch::DrawFilledRect(
                    scaledPos, 
                    ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + titlebarHeightScaled), 
                    ImColor(ImGui::GetColorU32(isFocused ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBg))
                );
            }

            // 3. Draw Title Text
            if (!Title.empty()) {
                SpriteBatch::DrawString(Title, this->Font ? this->Font : nullptr, ImVec2(scaledPos.x + (15.0f * scale), scaledPos.y + (10.0f * scale)));
            }

            // 4. Custom Dragging Area (Titlebar)
            ImGui::SetCursorPos(ImVec2(0, 0));
            ImGui::InvisibleButton("##titleBarDrag", ImVec2(scaledSize.x, titlebarHeightScaled));
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                ImGui::SetWindowPos(ImVec2(scaledPos.x + delta.x, scaledPos.y + delta.y));
            }

            // 5. Content Container
            // We use a sub-child to trap children safely below the titlebar and prevent overlap
            ImGui::SetCursorPos(ImVec2(0, titlebarHeightScaled));
            if (ImGui::BeginChild("##windowContent", ImVec2(scaledSize.x, scaledSize.y - titlebarHeightScaled), false, ImGuiWindowFlags_NoBackground)) {
                ImVec2 scrolledPos = ImGui::GetCursorScreenPos();
                Rectangle clientBounds;
                clientBounds.X = scrolledPos.x;
                clientBounds.Y = scrolledPos.y;
                clientBounds.Width = scaledSize.x;
                clientBounds.Height = scaledSize.y - titlebarHeightScaled;
                DrawChildren(clientBounds);
            }
            ImGui::EndChild();

            // 6. Custom Resize Grip (Bottom Right)
            if (IsResizable) {
                float gripSizeScaled = 25.0f * scale;
                ImVec2 gripLocalPos = ImVec2(scaledSize.x - gripSizeScaled, scaledSize.y - gripSizeScaled);
                ImGui::SetCursorPos(gripLocalPos);
                
                ImGui::InvisibleButton("##resizeGrip", ImVec2(gripSizeScaled, gripSizeScaled));
                bool isHovered = ImGui::IsItemHovered();
                bool isActive = ImGui::IsItemActive();

                std::shared_ptr<AsyncTexture> resizeTex = ResizeInactiveTexture;
                if (isActive) resizeTex = ResizeActiveTexture;
                else if (isHovered) resizeTex = ResizeHoverTexture;

                if (resizeTex && resizeTex->Get()) {
                    SpriteBatch::DrawTexture(
                        resizeTex.get(),
                        ImVec2(scaledPos.x + gripLocalPos.x, scaledPos.y + gripLocalPos.y),
                        ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y)
                    );
                } else {
                    // Fallback to a simple drawn triangle
                    ImColor gripColor(ImGui::GetColorU32(isActive ? ImGuiCol_ResizeGripActive : (isHovered ? ImGuiCol_ResizeGripHovered : ImGuiCol_ResizeGrip)));
                    SpriteBatch::DrawTriangleFilled(
                        ImVec2(scaledPos.x + scaledSize.x - gripSizeScaled, scaledPos.y + scaledSize.y),
                        ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y),
                        ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y - gripSizeScaled),
                        gripColor
                    );
                }

                // Handle manual resizing
                if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    ImVec2 delta = ImGui::GetIO().MouseDelta;
                    ImVec2 newSize = ImVec2((std::max)(100.0f * scale, scaledSize.x + delta.x), (std::max)(100.0f * scale, scaledSize.y + delta.y));
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
