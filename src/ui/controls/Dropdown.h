#ifndef NEXUS_SDK_UI_DROPDOWN_H
#define NEXUS_SDK_UI_DROPDOWN_H

#include "ControlBase.h"
#include "Label.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace NexusSDK {
    class AsyncTexture;
    
namespace UI {

    class Dropdown : public ControlBase {
    public:
        std::string m_id;
        std::vector<std::string> Options;
        std::vector<std::shared_ptr<Label>> OptionLabels;
        int* SelectedIndex;
        std::shared_ptr<Label> TextLabel;
        std::function<void(int)> OnSelectionChanged;

        Dropdown(const char* id, int* selectedIndex, const std::vector<std::string>& options, const char* text = "")
            : m_id(id), SelectedIndex(selectedIndex), Options(options) {
            if (text && text[0] != '\0') {
                TextLabel = std::make_shared<Label>(text);
                TextLabel->WrapText = false;
            }
            for (const auto& opt : options) {
                auto lbl = std::make_shared<Label>(opt);
                lbl->WrapText = false;
                OptionLabels.push_back(lbl);
            }
        }

        void OnDraw(const Rectangle& bounds, float scale) override;

        virtual ImVec2 GetAutoSize(float scale) const override {
            ImVec2 size(150.0f, ImGui::GetTextLineHeightWithSpacing() + 4.0f);
            if (TextLabel) {
                float spacing = 8.0f;
                ImVec2 textSize = TextLabel->CalcSize();
                size.x += (textSize.x / scale) + spacing;
            }
            return size;
        }


    };

}
}

#endif
