#include "Menu.h"
#include <NexusSDK.h>
#include "../../utils/ImStateGuards.h"

#include <cstdlib>

namespace NexusSDK {
namespace UI {

void Menu::OnDraw(const Rectangle& bounds, float scale) {
    ImGui::SetCursorScreenPos(bounds.GetMin());

    Color bgGuard(BackgroundColor.w > 0.0f, ImGuiCol_ChildBg, BackgroundColor);
    
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
        
            Color headerGuard(isSelected, ImGuiCol_Header, 0.0f, 0.0f, 0.0f, 0.0f);
            Color headerHoveredGuard(ImGuiCol_HeaderHovered, 1.0f, 1.0f, 1.0f, 0.05f);
            Color headerActiveGuard(isSelected, ImGuiCol_HeaderActive, 1.0f, 1.0f, 1.0f, 0.05f);
            Color textGuard(ImGuiCol_Text, isSelected ? AccentColor : TextUnselectedColor);

            std::string label = "    " + Tabs[i].Name;
            Style alignGuard(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
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
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(cursorPos.x, cursorPos.y), 
                    ImVec2(cursorPos.x + (3.0f * scale), cursorPos.y + scaledItemHeight), 
                    ImGui::GetColorU32(AccentColor)
                );
            }
        } // closes for loop
    } // closes if (isVisible)
    
    ImGui::EndChild();
}

} // namespace UI
} // namespace NexusSDK
