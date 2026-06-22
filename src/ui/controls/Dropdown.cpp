#include "Dropdown.h"
#include "../../../NexusSDK.h"
#include "../../../nexus-sdk-resource.h"

#include <imgui/imgui.h>

namespace NexusSDK {
namespace UI {



    void Dropdown::OnRender() {
        if (!SelectedIndex) return;



        float height = ImGui::GetTextLineHeightWithSpacing() + 4.0f;

        if (TextLabel) {
            float currentY = ImGui::GetCursorPosY();
            float offset = (height - ImGui::GetFontSize()) / 2.0f;
            ImGui::SetCursorPosY(currentY + offset);
            
            TextLabel->Render();
            
            ImGui::SameLine();
            ImGui::SetCursorPosY(currentY);
        }

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = m_size.x > 0.0f ? m_size.x : ImGui::CalcItemWidth();
        if (width <= 0) width = 150.0f; // Default

        std::string popupId = "popup_" + m_id;

        // Button to toggle popup
        if (ImGui::InvisibleButton(m_id.c_str(), ImVec2(width, height))) {
            if (!ImGui::IsPopupOpen(popupId.c_str())) {
                ImGui::OpenPopup(popupId.c_str());
                NexusSDK::Audio->Play(IDR_AUDIO_CLICK);
            } else {
                ImGui::CloseCurrentPopup();
            }
        }

        bool isHovered = ImGui::IsItemHovered();
        bool isOpen = ImGui::IsPopupOpen(popupId.c_str());
        bool isHighlighted = isHovered || isOpen;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        // Background
        drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(10, 10, 10, 255));
        
        // Highlight tint
        if (isHighlighted) {
            drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(45, 37, 25, 255));
        }

        // 1-pixel border
        drawList->AddRect(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(80, 80, 80, 255), 0.0f, 0, 1.0f);

        // Selected Text
        if (*SelectedIndex >= 0 && *SelectedIndex < OptionLabels.size() && OptionLabels[*SelectedIndex]) {
            ImGui::SetCursorScreenPos(ImVec2(pos.x + 8.0f, pos.y + (height - ImGui::GetFontSize()) / 2.0f));
            if (isHighlighted) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(239/255.0f, 222/255.0f, 161/255.0f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(239/255.0f, 240/255.0f, 239/255.0f, 1.0f));
            }
            OptionLabels[*SelectedIndex]->Render();
            ImGui::PopStyleColor();
        }

        // Arrow
        Texture_t* texArrow = NexusSDK::Content->GetTexture(IDB_DROPDOWN_ARROW);
        float arrowSize = 16.0f;
        ImVec2 arrowMin = ImVec2(pos.x + width - arrowSize - 4.0f, pos.y + (height - arrowSize) / 2.0f);
        ImVec2 arrowMax = ImVec2(arrowMin.x + arrowSize, arrowMin.y + arrowSize);
        if (texArrow) {
            drawList->AddImage((ImTextureID)texArrow->Resource, arrowMin, arrowMax);
        }

        // Popup logic
        ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + height));
        ImGui::SetNextWindowSize(ImVec2(width, 0));
        
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.04f, 0.04f, 0.04f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        if (ImGui::BeginPopup(popupId.c_str())) {
            bool isPopupHovered = ImGui::IsWindowHovered();

            for (int i = 0; i < Options.size(); ++i) {
                ImGui::PushID(i);
                ImVec2 itemPos = ImGui::GetCursorScreenPos();
                ImVec2 itemSize = ImVec2(width, height);

                std::string itemId = "##item_" + m_id;
                bool itemClicked = ImGui::InvisibleButton(itemId.c_str(), itemSize);
                bool itemHovered = ImGui::IsItemHovered();

                if (itemClicked) {
                    *SelectedIndex = i;
                    if (OnSelectionChanged) OnSelectionChanged(*SelectedIndex);
                    NexusSDK::Audio->Play(IDR_AUDIO_CLICK);
                    ImGui::CloseCurrentPopup();
                }

                bool shouldHighlight = itemHovered || (!isPopupHovered && i == *SelectedIndex);

                ImDrawList* pdl = ImGui::GetWindowDrawList();
                if (shouldHighlight) {
                    pdl->AddRectFilled(itemPos, ImVec2(itemPos.x + itemSize.x, itemPos.y + itemSize.y), IM_COL32(45, 37, 25, 255));
                }
                
                ImGui::SetCursorScreenPos(ImVec2(itemPos.x + 8.0f, itemPos.y + (height - ImGui::GetFontSize()) / 2.0f));
                if (shouldHighlight) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(239/255.0f, 222/255.0f, 161/255.0f, 1.0f));
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(239/255.0f, 240/255.0f, 239/255.0f, 1.0f));
                }
                
                if (i < OptionLabels.size() && OptionLabels[i]) {
                    OptionLabels[i]->Render();
                }

                ImGui::PopStyleColor();

                ImGui::SetCursorScreenPos(ImVec2(itemPos.x, itemPos.y + height));
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }
        
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor(2);

        // Restore layout bounds so ImGui::SameLine() works on the dropdown box
        ImGui::SetCursorScreenPos(pos);
        ImGui::Dummy(ImVec2(width, height));
    }

}
}
