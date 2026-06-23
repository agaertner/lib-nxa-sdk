#pragma once

#include "ControlBase.h"
#include "Label.h"
#include <string>
#include <memory>

namespace NexusSDK {
namespace UI {

class Tooltip : public ControlBase {
public:
    Tooltip() : ControlBase() {}
    virtual ~Tooltip() = default;

    // Fast factory function for simple string tooltips
    static std::shared_ptr<Tooltip> CreateStringTooltip(const std::string& text) {
        auto tooltip = std::make_shared<Tooltip>();
        auto label = std::make_shared<Label>(text);
        tooltip->AddChild(label);
        return tooltip;
    }

protected:
    virtual void OnDraw(const Rectangle& bounds, float scale) override {
        ImGui::BeginTooltip();
        ImVec2 scrolledPos = ImGui::GetCursorScreenPos();
        Rectangle clientBounds;
        clientBounds.X = scrolledPos.x;
        clientBounds.Y = scrolledPos.y;
        clientBounds.Width = 1000.0f; // Arbitrary wide bounds for auto-resize
        clientBounds.Height = 1000.0f;
        DrawChildren(clientBounds, scale);
        ImGui::EndTooltip();
    }
};

} // namespace UI
} // namespace NexusSDK
