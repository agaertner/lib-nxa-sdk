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
    virtual void OnDraw(const Rectangle& bounds, float scale) override;
};

} // namespace UI
} // namespace NexusSDK
