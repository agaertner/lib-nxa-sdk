#include "../../../nexus-sdk-resource.h"
#include "StandardDialog.h"
#include "../../../NexusSDK.h"

#include <imgui/imgui.h>
#include <algorithm>

namespace NexusSDK {
namespace UI {

std::vector<std::shared_ptr<StandardDialog>> StandardDialog::s_activeDialogs;

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
    s_activeDialogs.push_back(dialog);
}

void StandardDialog::RenderAll() {
    for (auto it = s_activeDialogs.begin(); it != s_activeDialogs.end(); ) {
        (*it)->Render();
        if (!(*it)->IsVisible()) {
            it = s_activeDialogs.erase(it);
        } else {
            ++it;
        }
    }
}

void StandardDialog::Close() {
    SetVisible(false);
    ImGui::CloseCurrentPopup();
}

void StandardDialog::CalculateLayout() {
    if (m_layoutCalculated) return;

    m_maxIconSize = (m_icon == DialogIcon::None) ? 0.0f : 64.0f;
    float iconMargin = 5.0f;
    float rightPadding = 15.0f;

    // Auto-size buttons to match text if needed
    for (size_t i = 0; i < m_children.size(); ++i) {
        float textW = ImGui::CalcTextSize(m_buttonsData[i].Text.c_str()).x + rightPadding * 2.0f;
        if (textW > m_buttonWidth) {
            m_buttonWidth = textW;
        }
    }
    
    // Ensure the dialog is wide enough for all buttons
    float totalButtonWidth = (m_children.size() * m_buttonWidth) + ((m_children.size() - 1) * 3.0f) + rightPadding * 2.0f;
    if (totalButtonWidth > m_size.x) {
        m_size.x = totalButtonWidth;
    }

    m_layoutCalculated = true;
}

void StandardDialog::OnRender() {
    CalculateLayout();

    if (!ImGui::IsPopupOpen(m_id.c_str())) {
        ImGui::OpenPopup(m_id.c_str());
    }

    ImGui::SetNextWindowSize(ImVec2(m_size.x, 0)); // Let height auto-size
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    if (ImGui::BeginPopupModal(m_id.c_str(), &m_visible, flags)) {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);

        // Background
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

        float topPad = 7.0f;
        float rightPad = 15.0f;
        float leftPad = 5.0f;
        float bottomPad = 10.0f;
        
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
                
                ImGui::Image((ImTextureID)iconAtlas->Resource, ImVec2(m_maxIconSize, m_maxIconSize), uv0, uv1);
            }
        }

        // Label
        float textStartX = startX + m_maxIconSize + 25.0f;
        float textStartY = 17.0f;
        if (m_icon == DialogIcon::None) {
            textStartX = 15.0f;
        }
        
        ImGui::SetCursorPos(ImVec2(textStartX, textStartY));
        
        float wrapWidth = m_size.x - rightPad - textStartX;
        m_label->SetSize(ImVec2(wrapWidth, 0));
        m_label->Render();

        float currentY = ImGui::GetCursorPosY();
        if (currentY < startY + m_maxIconSize) {
            currentY = startY + m_maxIconSize;
        }
        
        currentY += 15.0f; // Space before buttons
        ImGui::SetCursorPosY(currentY);

        // Force the window to be EXACTLY m_size.x wide, so the right padding isn't clipped by auto-resize
        ImGui::SetCursorPosX(m_size.x - 1.0f);
        ImGui::Dummy(ImVec2(1.0f, 0.0f));

        // Buttons
        float buttonsTotalWidth = (m_children.size() * m_buttonWidth) + ((m_children.size() - 1) * 3.0f);
        float buttonsStartX = m_size.x - rightPad - buttonsTotalWidth;
        
        ImGui::SetCursorPosX(buttonsStartX);
        
        for (size_t i = 0; i < m_children.size(); ++i) {
            if (auto btn = std::dynamic_pointer_cast<Button>(m_children[i])) {
                btn->Width = m_buttonWidth;
                btn->Render();
                if (i < m_children.size() - 1) {
                    ImGui::SameLine(0, 3.0f);
                }
            }
        }

        ImGui::Dummy(ImVec2(0, bottomPad));

        ImGui::EndPopup();
    } else {
        ImGui::PopStyleVar(2);
    }
}

} // namespace UI
} // namespace NexusSDK
