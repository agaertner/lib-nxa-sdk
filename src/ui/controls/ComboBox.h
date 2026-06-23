#pragma once

#include "ControlBase.h"
#include <string>
#include <vector>

namespace NexusSDK {
namespace UI {

class ComboBox : public ControlBase {
public:
    ComboBox() : ControlBase() {}
    ComboBox(const std::string& text, int* selectedIndex, const std::vector<std::string>& options) 
        : ControlBase(), Text(text), SelectedIndex(selectedIndex), Options(options) {}
    virtual ~ComboBox() = default;

    std::string Text = "";
    int* SelectedIndex = nullptr;
    std::vector<std::string> Options;
    std::function<void(int)> OnValueChanged;

protected:
    virtual void OnDraw(const Rectangle& bounds, float scale) override {
        if (!SelectedIndex || Options.empty() || *SelectedIndex < 0 || *SelectedIndex >= (int)Options.size()) return;

        ImGui::SetCursorScreenPos(bounds.GetMin());
        ImGui::SetNextItemWidth(bounds.Width);
        if (ImGui::BeginCombo(Text.c_str(), Options[*SelectedIndex].c_str())) {
            for (size_t i = 0; i < Options.size(); i++) {
                bool isSelected = (*SelectedIndex == static_cast<int>(i));
                if (ImGui::Selectable(Options[i].c_str(), isSelected)) {
                    if (OnValueChanged) {
                        OnValueChanged(static_cast<int>(i));
                    }
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
};

} // namespace UI
} // namespace NexusSDK
