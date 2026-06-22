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
            }
            for (const auto& opt : options) {
                auto lbl = std::make_shared<Label>(opt);
                lbl->WrapText = false;
                OptionLabels.push_back(lbl);
            }
        }

        void OnRender() override;

    private:
        static std::shared_ptr<AsyncTexture> s_texArrow;
        static void InitializeTextures();
    };

}
}

#endif
