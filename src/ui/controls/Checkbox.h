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
    }
    virtual ~Checkbox() = default;

    std::shared_ptr<Label> TextLabel;
    bool* Value = nullptr;
    std::function<void(bool)> OnCheckedChanged;

protected:
    virtual void OnRender() override;

private:

};

} // namespace UI
} // namespace NexusSDK
