#include "../../../nexus-sdk-resource.h"
#include "StandardDialog.h"
#include "../../../NexusSDK.h"

#include <imgui/imgui.h>
#include <algorithm>
#include "../Viewport.h"

namespace NexusSDK {
namespace UI {

DialogButton DialogButton::OK() { return {NexusSDK::SDKLocal ? NexusSDK::SDKLocal->GetString("Action_OK") : "OK", true}; }
DialogButton DialogButton::Confirm() { return {NexusSDK::SDKLocal ? NexusSDK::SDKLocal->GetString("Action_Confirm") : "Confirm", true}; }
DialogButton DialogButton::Accept() { return {NexusSDK::SDKLocal ? NexusSDK::SDKLocal->GetString("Action_Accept") : "Accept"}; }
DialogButton DialogButton::Cancel() { return {NexusSDK::SDKLocal ? NexusSDK::SDKLocal->GetString("Action_Cancel") : "Cancel"}; }
DialogButton DialogButton::Yes() { return {NexusSDK::SDKLocal ? NexusSDK::SDKLocal->GetString("Action_Yes") : "Yes"}; }
DialogButton DialogButton::No() { return {NexusSDK::SDKLocal ? NexusSDK::SDKLocal->GetString("Action_No") : "No"}; }
DialogButton DialogButton::Ignore() { return {NexusSDK::SDKLocal ? NexusSDK::SDKLocal->GetString("Action_Ignore") : "Ignore"}; }
DialogButton DialogButton::Close() { return {NexusSDK::SDKLocal ? NexusSDK::SDKLocal->GetString("Action_Close") : "Close"}; }

StandardDialog::StandardDialog(const std::string& text, DialogIcon sysIcon, const std::vector<DialogButton>& buttons)
    : m_text(text), m_icon(sysIcon) {
    
    m_size = ImVec2(454.0f, 100.0f);
    
    m_buttonsData = buttons;
    if (m_buttonsData.empty()) {
        m_buttonsData.push_back(DialogButton::OK());
    }

    m_label = std::make_shared<Label>(m_text);

    for (size_t i = 0; i < m_buttonsData.size(); ++i) {
        auto& btnData = m_buttonsData[i];
        auto btn = std::make_shared<Button>("btn_" + std::to_string(i), btnData.Text);
        btn->Width = m_buttonWidth;
        btn->Height = m_buttonHeight;
        btn->OnClick = [this, btnData]() {
            if (btnData.OnClick) btnData.OnClick();
            Close();
        };
        AddChild(btn);
    }
}

void StandardDialog::Show(const std::string& text, DialogIcon sysIcon, std::vector<DialogButton> buttons) {
    if (NexusSDK::Audio) {
        NexusSDK::Audio->Play("NXA_AUDIO_MENU_ITEM_CLICK");
    }
    auto dialog = std::make_shared<StandardDialog>(text, sysIcon, buttons);
    
    if (Viewport::Get()) {
        Viewport::Get()->AddChild(dialog);
    }
}

void StandardDialog::Close() {
    SetVisible(false);
    ImGui::CloseCurrentPopup();
    
    if (auto parent = GetParent()) {
        parent->RemoveChild(shared_from_this());
    }
}

void StandardDialog::CalculateLayout(float scale) {
    if (m_layoutCalculated) return;

    m_maxIconSize = (m_icon == DialogIcon::None) ? 0.0f : 64.0f;
    float iconMargin = 5.0f;
    float rightPadding = 15.0f;

    for (size_t i = 0; i < m_children.size(); ++i) {
        float textW = (ImGui::CalcTextSize(m_buttonsData[i].Text.c_str()).x / scale) + rightPadding * 2.0f;
        if (textW > m_buttonWidth) {
            m_buttonWidth = textW;
        }
    }
    
    float totalButtonWidth = (m_children.size() * m_buttonWidth) + ((m_children.size() - 1) * 3.0f) + rightPadding * 2.0f;
    if (totalButtonWidth > m_size.x) {
        m_size.x = totalButtonWidth;
    }

    m_layoutCalculated = true;
}

void StandardDialog::Draw(const Rectangle& bounds, float scale) {
    if (!m_visible) return;

    // Create an invisible, non-interactive host window for the modal
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(0,0));
    if (ImGui::Begin(m_id.c_str(), nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
        
        if (!m_wasOpened) {
            ImGui::OpenPopup((m_id + "_Popup").c_str());
            m_wasOpened = true;
        }

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); 
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        ImGui::SetNextWindowSize(ImVec2(m_size.x * scale, 0)); // Let height auto-size
        if (ImGui::BeginPopupModal((m_id + "_Popup").c_str(), &m_visible, flags)) {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);

            OnDraw(bounds, scale);

            ImGui::EndPopup();
        } else {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
        }
    }
    ImGui::End();
}

void StandardDialog::OnDraw(const Rectangle& bounds, float scale) {
    CalculateLayout(scale);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    Texture_t* bgTex = NexusSDK::Content->GetTexture(IDB_DIALOG_BG);
        if (bgTex) {
            ImVec2 p_min = ImGui::GetWindowPos();
            ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowSize().x, p_min.y + ImGui::GetWindowSize().y);
            
            float windowW = ImGui::GetWindowSize().x;
            float windowH = ImGui::GetWindowSize().y;
            float texW = (float)bgTex->Width;
            float texH = (float)bgTex->Height;

            float startX = 33.0f;
            float startY = 27.0f;
            float maxW = 936.0f;
            float maxH = 936.0f;

            float srcW = windowW < maxW ? windowW : maxW;
            float srcH = windowH < maxH ? windowH : maxH;

            ImVec2 uv0 = ImVec2(startX / texW, startY / texH);
            ImVec2 uv1 = ImVec2((startX + srcW) / texW, (startY + srcH) / texH);
            
            ImGui::GetWindowDrawList()->AddImage((ImTextureID)bgTex->Resource, p_min, p_max, uv0, uv1, ImGui::GetColorU32(IM_COL32_WHITE));
        }
        
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), ImGui::GetColorU32(IM_COL32(0, 0, 0, 255)), 0.0f, 0, 2.0f);

        float topPad = 7.0f * scale;
        float rightPad = 15.0f * scale;
        float leftPad = 5.0f * scale;
        float bottomPad = 10.0f * scale;
        
        ImGui::SetCursorPos(ImVec2(leftPad, topPad));
        
        float startX = ImGui::GetCursorPosX();
        float startY = ImGui::GetCursorPosY();

        // Icon
        if (m_icon != DialogIcon::None) {
            Texture_t* iconAtlas = NexusSDK::Content->GetTexture(IDB_DIALOG_ICONS);
            if (iconAtlas) {
                ImVec2 uv0 = ImVec2(0, 0);
                ImVec2 uv1 = ImVec2(64.0f / iconAtlas->Width, 64.0f / iconAtlas->Height);
                
                if (m_icon == DialogIcon::Question) {
                    uv0.x = 64.0f / iconAtlas->Width;
                    uv1.x = 128.0f / iconAtlas->Width;
                } else if (m_icon == DialogIcon::Present) {
                    uv0.x = 128.0f / iconAtlas->Width;
                    uv1.x = 192.0f / iconAtlas->Width;
                }
                
                ImGui::Image((ImTextureID)iconAtlas->Resource, ImVec2(m_maxIconSize * scale, m_maxIconSize * scale), uv0, uv1);
            }
        }

        // Label
        float textStartX = startX + (m_maxIconSize * scale) + (25.0f * scale);
        float textStartY = 17.0f * scale;
        if (m_icon == DialogIcon::None) {
            textStartX = startX + (15.0f * scale);
        }
        
        ImGui::SetCursorPos(ImVec2(textStartX, textStartY));
        
        float wrapWidth = (m_size.x * scale) - rightPad - textStartX;
        
        Rectangle labelBounds;
        labelBounds.X = ImGui::GetCursorScreenPos().x;
        labelBounds.Y = ImGui::GetCursorScreenPos().y;
        labelBounds.Width = wrapWidth;
        labelBounds.Height = 0; // Let Label calc its own height
        m_label->WrapText = true;
        m_label->Draw(labelBounds, scale);

        float currentY = ImGui::GetCursorPosY();
        if (currentY < startY + (m_maxIconSize * scale)) {
            currentY = startY + (m_maxIconSize * scale);
        }
        
        currentY += 15.0f * scale; // Space before buttons
        ImGui::SetCursorPosY(currentY);

        // Force the window to be EXACTLY m_size.x * scale wide
        ImGui::SetCursorPosX((m_size.x * scale) - 1.0f);
        ImGui::Dummy(ImVec2(1.0f, 0.0f));

        // Buttons
        float buttonsTotalWidthScaled = (m_children.size() * m_buttonWidth * scale) + ((m_children.size() - 1) * 3.0f * scale);
        
        // Push cursor to bottom
        currentY = ImGui::GetCursorPosY();
        float targetY = (m_size.y * scale) - (m_buttonHeight * scale) - bottomPad;
        if (targetY > currentY) {
            ImGui::SetCursorPosY(targetY);
        }

        float buttonsStartXScaled = (m_size.x * scale) - rightPad - buttonsTotalWidthScaled;
        ImGui::SetCursorPosX(buttonsStartXScaled);

        for (size_t i = 0; i < m_children.size(); ++i) {
            if (auto btn = std::dynamic_pointer_cast<Button>(m_children[i])) {
                btn->Width = m_buttonWidth;
                
                Rectangle childBounds;
                childBounds.X = ImGui::GetCursorScreenPos().x;
                childBounds.Y = ImGui::GetCursorScreenPos().y;
                childBounds.Width = m_buttonWidth * scale;
                childBounds.Height = m_buttonHeight * scale;

                ImGui::SetCursorScreenPos(ImVec2(childBounds.X, childBounds.Y));
                
                // We inline the interaction and rendering to bypass the bizarre ControlBase Modal clipping anomaly
                bool isHovered = false;
                bool isClicked = ImGui::InvisibleButton(("##InvBtn" + btn->GetID()).c_str(), ImVec2(childBounds.Width, childBounds.Height));
                
                if (ImGui::IsItemHovered()) {
                    isHovered = true;
                }

                if (isClicked) {
                    if (btn->OnClick) btn->OnClick();
                    NexusSDK::Audio->Play(IDR_AUDIO_BUTTON_CLICK);
                }

                // Force anim state into the button so it can track time
                btn->ForceHover = isHovered;
                float dt = ImGui::GetIO().DeltaTime;
                
                // Directly manage a local animation state for immediate rendering
                static std::map<std::string, float> s_animStates;
                std::string btnKey = m_id + "_" + btn->GetID();
                if (isHovered) {
                    s_animStates[btnKey] += dt * (8.0f / 0.25f);
                } else {
                    s_animStates[btnKey] -= dt * (8.0f / 0.25f);
                }
                if (s_animStates[btnKey] > 8.0f) s_animStates[btnKey] = 8.0f;
                if (s_animStates[btnKey] < 0.0f) s_animStates[btnKey] = 0.0f;

                int frame = static_cast<int>(s_animStates[btnKey]);
                if (frame >= 8) frame = 7;
                if (ImGui::IsItemActive() && isHovered) {
                    frame = 7; // Pressed state
                }

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                Texture_t* texStates = NexusSDK::Content->GetTexture(IDB_BUTTON_STATES);
                Texture_t* texBorder = NexusSDK::Content->GetTexture(IDB_BUTTON_BORDER);

                ImVec2 pos = ImVec2(childBounds.X, childBounds.Y);
                ImVec2 size = ImVec2(childBounds.Width, childBounds.Height);

                if (texStates) {
                    float totalWidth = static_cast<float>(texStates->Width);
                    float totalHeight = static_cast<float>(texStates->Height);
                    float ATLAS_SPRITE_WIDTH = 350.0f;
                    float ATLAS_SPRITE_HEIGHT = 20.0f;
                    
                    float uv0x = (frame * ATLAS_SPRITE_WIDTH) / totalWidth;
                    float uv0y = 0.0f;
                    float uv1x = ((frame + 1) * ATLAS_SPRITE_WIDTH) / totalWidth;
                    float uv1y = ATLAS_SPRITE_HEIGHT / totalHeight;

                    drawList->AddImage((ImTextureID)texStates->Resource, 
                                       ImVec2(pos.x + 3, pos.y + 3), 
                                       ImVec2(pos.x + size.x - 3, pos.y + size.y - 2), 
                                       ImVec2(uv0x, uv0y), ImVec2(uv1x, uv1y), ImGui::GetColorU32(IM_COL32_WHITE));
                }

                if (texBorder) {
                    float bw = 4.0f; float bh = 4.0f;
                    // Top
                    drawList->AddImage((ImTextureID)texBorder->Resource,
                                       ImVec2(pos.x + 2, pos.y), ImVec2(pos.x + size.x - 3, pos.y + 4),
                                       ImVec2(0/bw, 0/bh), ImVec2(1/bw, 4/bh), ImGui::GetColorU32(IM_COL32_WHITE));
                    // Right
                    drawList->AddImage((ImTextureID)texBorder->Resource,
                                       ImVec2(pos.x + size.x - 4, pos.y + 2), ImVec2(pos.x + size.x, pos.y + size.y - 1),
                                       ImVec2(0/bw, 1/bh), ImVec2(4/bw, 2/bh), ImGui::GetColorU32(IM_COL32_WHITE));
                    // Bottom
                    drawList->AddImage((ImTextureID)texBorder->Resource,
                                       ImVec2(pos.x + 3, pos.y + size.y - 4), ImVec2(pos.x + size.x - 3, pos.y + size.y),
                                       ImVec2(1/bw, 0/bh), ImVec2(2/bw, 4/bh), ImGui::GetColorU32(IM_COL32_WHITE));
                    // Left
                    drawList->AddImage((ImTextureID)texBorder->Resource,
                                       ImVec2(pos.x, pos.y + 2), ImVec2(pos.x + 4, pos.y + size.y - 1),
                                       ImVec2(0/bw, 3/bh), ImVec2(4/bw, 4/bh), ImGui::GetColorU32(IM_COL32_WHITE));
                }

                if (btn->TextLabel) {
                    ImVec2 textSize = btn->TextLabel->CalcSize();
                    float textX = pos.x + (size.x - textSize.x) / 2.0f;
                    float textY = pos.y + (size.y - textSize.y) / 2.0f;

                    Rectangle textBounds;
                    textBounds.X = textX; textBounds.Y = textY;
                    textBounds.Width = textSize.x; textBounds.Height = textSize.y;

                    btn->TextLabel->Draw(textBounds, scale);
                }

                // Restore cursor for next button
                ImGui::SetCursorScreenPos(ImVec2(childBounds.X + childBounds.Width + 3.0f * scale, childBounds.Y));
            }
        }

        ImGui::Dummy(ImVec2(0, m_buttonHeight * scale + 10.0f));
}

} // namespace UI
} // namespace NexusSDK
