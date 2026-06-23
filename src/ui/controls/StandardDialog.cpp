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

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Center on screen
    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(m_size.x * scale, 0)); // Let height auto-size

    if (ImGui::Begin((m_id + "_Dialog").c_str(), &m_visible, flags)) {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);

        OnDraw(bounds, scale);
    } else {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
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

                // Standard clean rendering. Button handles its own state internally now.
                ImGui::SetCursorScreenPos(ImVec2(childBounds.X, childBounds.Y));
                btn->Draw(childBounds, scale);

                // Restore cursor for next button
                ImGui::SetCursorScreenPos(ImVec2(childBounds.X + childBounds.Width + 3.0f * scale, childBounds.Y));
            }
        }

        ImGui::Dummy(ImVec2(0, m_buttonHeight * scale + 10.0f));
}

} // namespace UI
} // namespace NexusSDK
