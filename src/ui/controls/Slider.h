#pragma once

#include "ControlBase.h"
#include "../../utils/AsyncTexture.h"
#include "Label.h"
#include <string>

namespace NexusSDK {
namespace UI {

class Slider : public ControlBase {
public:
    Slider() : ControlBase() {}
    Slider(const std::string& text, float* value, float min, float max) 
        : ControlBase(), Value(value), Min(min), Max(max) {
        TextLabel = std::make_shared<Label>(text);
    }
    virtual ~Slider() = default;

    std::shared_ptr<Label> TextLabel;
    float* Value = nullptr;
    float Min = 0.0f;
    float Max = 1.0f;
    float LabelWidth = 0.0f;
    std::string Format = "%.2f";
    std::function<void(float)> OnValueChanged;

protected:
    virtual void OnRender() override;

private:

};

} // namespace UI
} // namespace NexusSDK
