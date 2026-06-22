#pragma once

#include "ControlBase.h"
#include <string>
#include <vector>
#include <functional>
#include <imgui/imgui.h>

namespace NexusSDK {
namespace UI {

class Menu : public ControlBase {
public:
    struct MenuTab {
        std::string Name;
        std::string Title;
        std::shared_ptr<ControlBase> PageControl;
        std::function<void()> OnSelected;
    };

    Menu() : ControlBase() {}
    virtual ~Menu() = default;

    std::vector<MenuTab> Tabs;
    int SelectedIndex = -1;
    std::function<void(int)> OnSelectionChanged;

    // Styling Properties
    float ItemHeight = 24.0f;
    ImVec4 AccentColor = {1.0f, 1.0f, 1.0f, 1.0f}; // Fallback to white if not set
    ImVec4 TextUnselectedColor = {0.6f, 0.6f, 0.6f, 1.0f};
    bool ShowAccentBar = true;
    
    // Header
    std::string HeaderTitle = "";
    std::string HeaderSubtitle = "";
    ImVec4 BackgroundColor = {0,0,0,0};

    void AddTab(const MenuTab& tab) {
        Tabs.push_back(tab);
    }

protected:
    virtual void OnRender() override {
        if (BackgroundColor.w > 0.0f) {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundColor);
        }
        
        bool isVisible = ImGui::BeginChild(m_id.c_str(), m_size, true, BackgroundColor.w == 0.0f ? ImGuiWindowFlags_NoBackground : 0);
        
        if (BackgroundColor.w > 0.0f) {
            ImGui::PopStyleColor();
        }

        if (isVisible) {
            if (!HeaderTitle.empty()) {
                ImGui::Spacing();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);
                ImGui::TextColored(AccentColor, HeaderTitle.c_str());
                
                if (!HeaderSubtitle.empty()) {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);
                    ImGui::SetWindowFontScale(0.80f);
                    ImGui::TextDisabled("%s", HeaderSubtitle.c_str());
                    ImGui::SetWindowFontScale(1.0f);
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }

            for (size_t i = 0; i < Tabs.size(); ++i) {
                bool isSelected = (SelectedIndex == static_cast<int>(i));
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0,0,0,0));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1,1,1,0.05f));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1,1,1,0.05f));
                ImGui::PushStyleColor(ImGuiCol_Text, AccentColor);
            } else {
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1,1,1,0.05f));
                ImGui::PushStyleColor(ImGuiCol_Text, TextUnselectedColor);
            }

            std::string label = "    " + Tabs[i].Name;
            ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
            if (ImGui::Selectable(label.c_str(), isSelected, 0, ImVec2(0, ItemHeight))) {
                if (SelectedIndex != static_cast<int>(i)) {
                    SelectedIndex = static_cast<int>(i);
                    if (Tabs[i].OnSelected) Tabs[i].OnSelected();
                    if (OnSelectionChanged) OnSelectionChanged(SelectedIndex);
                }
            }
            ImGui::PopStyleVar();

            if (isSelected && ShowAccentBar) {
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(cursorPos.x, cursorPos.y), 
                    ImVec2(cursorPos.x + 3.0f, cursorPos.y + ItemHeight), 
                    ImGui::GetColorU32(AccentColor)
                );
            }

            if (isSelected) {
                ImGui::PopStyleColor(4);
            } else {
                ImGui::PopStyleColor(2);
            }
        } // closes for loop
        } // closes if (isVisible)
        
        ImGui::EndChild();
    }
};

} // namespace UI
} // namespace NexusSDK
