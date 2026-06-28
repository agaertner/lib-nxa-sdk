#pragma once

#include "ControlBase.h"
#include "../../utils/AsyncTexture.h"
#include "Label.h"
#include <string>

namespace NexusSDK {
namespace UI {

class Checkbox : public ControlBase {
public:
    Checkbox() : ControlBase() {}
    Checkbox(const std::string& text, bool* value) : ControlBase(), Value(value) {
        TextLabel = std::make_shared<Label>(text);
        TextLabel->WrapText = false;
    }
    virtual ~Checkbox() = default;

    std::shared_ptr<Label> TextLabel;
    bool* Value = nullptr;
    std::function<void(bool)> OnCheckedChanged;

protected:
    virtual void OnDraw(const Rectangle& bounds) override;

    virtual ImVec2 GetAutoSize() const override {
        ImVec2 size(0, 32.0f);
        if (TextLabel) {
            float spacing = 4.0f;
            ImVec2 textSize = TextLabel->CalcSize();
            size.x = 32.0f + spacing + (textSize.x / UIScale::Get());
        } else {
            size.x = 32.0f;
        }
        return size;
    }

private:

};

} // namespace UI
} // namespace NexusSDK
