#include "Menu.h"
#include <NexusSDK.h>
#include "../../utils/ImStateGuards.h"
#include "../SpriteBatch.h"

#include <cstdlib>

namespace NexusSDK {
namespace UI {

void Menu::OnDraw(const Rectangle& bounds) {
    float scale = UIScale::Get();
    ColorGuard bg(BackgroundColor.w > 0.0f, ImGuiCol_ChildBg, BackgroundColor);
    
    bool isVisible = ImGui::BeginChild(m_id.c_str(), ImVec2(bounds.Width, bounds.Height), true, BackgroundColor.w == 0.0f ? ImGuiWindowFlags_NoBackground : 0);

    if (isVisible) {
        if (!HeaderTitle.empty()) {
            ImGui::Spacing();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (8.0f * scale));
            ImGui::TextColored(AccentColor, HeaderTitle.c_str());
            
            if (!HeaderSubtitle.empty()) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (8.0f * scale));
                ImGui::SetWindowFontScale(0.80f);
                ImGui::TextDisabled("%s", HeaderSubtitle.c_str());
                ImGui::SetWindowFontScale(1.0f);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        float scaledItemHeight = ItemHeight * scale;

        for (size_t i = 0; i < Tabs.size(); ++i) {
            bool isSelected = (SelectedIndex == static_cast<int>(i));
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        
            ColorGuard header(isSelected, ImGuiCol_Header, 0.0f, 0.0f, 0.0f, 0.0f);
            ColorGuard headerHovered(ImGuiCol_HeaderHovered, 1.0f, 1.0f, 1.0f, 0.05f);
            ColorGuard headerActive(isSelected, ImGuiCol_HeaderActive, 1.0f, 1.0f, 1.0f, 0.05f);
            ColorGuard text(ImGuiCol_Text, isSelected ? AccentColor : TextUnselectedColor);

            std::string label = "    " + Tabs[i].Name;
            StyleGuard align(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
            if (ImGui::Selectable(label.c_str(), isSelected, 0, ImVec2(0, scaledItemHeight))) {
                if (SelectedIndex != static_cast<int>(i)) {
                    SelectedIndex = static_cast<int>(i);

                    const char* sounds[] = { "NXA_WAV_MENUCLICK1", "NXA_WAV_MENUCLICK2", "NXA_WAV_MENUCLICK3", "NXA_WAV_MENUCLICK4" };
                    if (NexusSDK::Audio) {
                        NexusSDK::Audio->Play(sounds[std::rand() % 4]);
                    }

                    if (Tabs[i].OnSelected) Tabs[i].OnSelected();
                    if (OnSelectionChanged) OnSelectionChanged(SelectedIndex);
                }
            }

            if (isSelected && ShowAccentBar) {
                SpriteBatch::DrawFilledRect(
                    ImVec2(cursorPos.x, cursorPos.y), 
                    ImVec2(cursorPos.x + (3.0f * scale), cursorPos.y + scaledItemHeight), 
                    ImColor(AccentColor)
                );
            }
        } // closes for loop
    } // closes if (isVisible)
    
    ImGui::EndChild();
}

} // namespace UI
} // namespace NexusSDK
