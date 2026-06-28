#include "../../nexus-sdk-resource.h"
#include "Dropdown.h"
#include <NexusSDK.h>
#include "../../utils/ImStateGuards.h"
#include "../SpriteBatch.h"


#include <imgui.h>

namespace NexusSDK {
namespace UI {



    void Dropdown::OnDraw(const Rectangle& bounds) {
        if (!SelectedIndex) return;
        float scale = UIScale::Get();

        float height = bounds.Height > 0.0f ? bounds.Height : (ImGui::GetTextLineHeightWithSpacing() + 4.0f) * scale;
        ImVec2 pos = bounds.GetMin();
        float width = bounds.Width > 0.0f ? bounds.Width : GetAutoSize().x * scale;

        float buttonWidth = width;
        if (TextLabel) {
            float spacing = 8.0f * scale;
            ImVec2 textSize = TextLabel->CalcSize();
            float offset = (height - textSize.y) / 2.0f;
            
            Rectangle textBounds;
            textBounds.X = pos.x;
            textBounds.Y = pos.y + offset;
            textBounds.Width = textSize.x;
            textBounds.Height = textSize.y;
            
            TextLabel->Draw(textBounds);
            
            // Shift pos to the right for the actual dropdown box
            pos.x += textSize.x + spacing;
            buttonWidth -= (textSize.x + spacing); 
        }

        ImGui::SetCursorScreenPos(pos);

        std::string popupId = "popup_" + m_id;

        // Button to toggle popup
        if (ImGui::InvisibleButton(m_id.c_str(), ImVec2(buttonWidth, height))) {
            if (!ImGui::IsPopupOpen(popupId.c_str())) {
                ImGui::OpenPopup(popupId.c_str());
                NexusSDK::Audio->Play(IDR_AUDIO_BUTTON_CLICK);
            } else {
                ImGui::CloseCurrentPopup();
            }
        }

        bool isHovered = ImGui::IsItemHovered();
        bool isOpen = ImGui::IsPopupOpen(popupId.c_str());
        bool isHighlighted = isHovered || isOpen;

        // Background
        SpriteBatch::DrawFilledRect(pos, ImVec2(pos.x + buttonWidth, pos.y + height), ImColor(10, 10, 10, 255));
        
        // Highlight tint
        if (isHighlighted) {
            SpriteBatch::DrawFilledRect(pos, ImVec2(pos.x + buttonWidth, pos.y + height), ImColor(45, 37, 25, 255));
        }

        // 1-pixel border
        SpriteBatch::DrawRect(pos, ImVec2(pos.x + buttonWidth, pos.y + height), ImColor(80, 80, 80, 255), 1.0f);

        // Selected Text
        if (*SelectedIndex >= 0 && *SelectedIndex < OptionLabels.size() && OptionLabels[*SelectedIndex]) {
            ImVec2 textSize = OptionLabels[*SelectedIndex]->CalcSize();
            Rectangle selBounds;
            selBounds.X = pos.x + (8.0f * scale);
            selBounds.Y = pos.y + (height - textSize.y) / 2.0f;
            selBounds.Width = textSize.x;
            selBounds.Height = textSize.y;

            ColorGuard text(ImGuiCol_Text, isHighlighted ? ImVec4(239/255.0f, 222/255.0f, 161/255.0f, 1.0f) : ImVec4(239/255.0f, 240/255.0f, 239/255.0f, 1.0f));
            OptionLabels[*SelectedIndex]->Draw(selBounds);
        }

        // Arrow
        AsyncTexture* texArrowAsync = NexusSDK::Content->GetTexture(IDB_DROPDOWN_ARROW);
        float arrowSize = 16.0f * scale;
        ImVec2 arrowMin = ImVec2(pos.x + buttonWidth - arrowSize - (4.0f * scale), pos.y + (height - arrowSize) / 2.0f);
        ImVec2 arrowMax = ImVec2(arrowMin.x + arrowSize, arrowMin.y + arrowSize);
        if (texArrowAsync) {
            SpriteBatch::DrawTexture(texArrowAsync, arrowMin, arrowMax);
        }

        // Popup logic
        ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + height));
        ImGui::SetNextWindowSize(ImVec2(buttonWidth, 0));
        
        ColorGuard popupBg(ImGuiCol_PopupBg, 0.04f, 0.04f, 0.04f, 1.0f);
        ColorGuard border(ImGuiCol_Border, 0.3f, 0.3f, 0.3f, 1.0f);
        StyleGuard borderSize(ImGuiStyleVar_WindowBorderSize, 1.0f);
        StyleGuard padding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        StyleGuard rounding(ImGuiStyleVar_PopupRounding, 0.0f);
        StyleGuard spacing(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        if (ImGui::BeginPopup(popupId.c_str())) {
            bool isPopupHovered = ImGui::IsWindowHovered();

            for (int i = 0; i < Options.size(); ++i) {
                IDGuard id(i);
                ImVec2 itemPos = ImGui::GetCursorScreenPos();
                ImVec2 itemSize = ImVec2(buttonWidth, height);

                std::string itemId = "##item_" + m_id;
                bool itemClicked = ImGui::InvisibleButton(itemId.c_str(), itemSize);
                bool itemHovered = ImGui::IsItemHovered();

                if (itemClicked) {
                    *SelectedIndex = i;
                    if (OnSelectionChanged) OnSelectionChanged(*SelectedIndex);
                    NexusSDK::Audio->Play(IDR_AUDIO_BUTTON_CLICK);
                    ImGui::CloseCurrentPopup();
                }

                bool shouldHighlight = itemHovered || (!isPopupHovered && i == *SelectedIndex);

                if (shouldHighlight) {
                    SpriteBatch::DrawFilledRect(itemPos, ImVec2(itemPos.x + itemSize.x, itemPos.y + itemSize.y), ImColor(45, 37, 25, 255));
                }
                
                ColorGuard text(ImGuiCol_Text, shouldHighlight ? ImVec4(239/255.0f, 222/255.0f, 161/255.0f, 1.0f) : ImVec4(239/255.0f, 240/255.0f, 239/255.0f, 1.0f));
                
                if (i < OptionLabels.size() && OptionLabels[i]) {
                    ImVec2 itemTextSize = OptionLabels[i]->CalcSize();
                    Rectangle itemSelBounds;
                    itemSelBounds.X = itemPos.x + (8.0f * scale);
                    itemSelBounds.Y = itemPos.y + (height - itemTextSize.y) / 2.0f;
                    itemSelBounds.Width = itemTextSize.x;
                    itemSelBounds.Height = itemTextSize.y;
                    OptionLabels[i]->Draw(itemSelBounds);
                }

                ImGui::SetCursorScreenPos(ImVec2(itemPos.x, itemPos.y + height));
            }
            ImGui::EndPopup();
        }
    }

}
}
