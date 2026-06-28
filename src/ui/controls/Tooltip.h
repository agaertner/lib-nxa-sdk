#pragma once

#include "Container.h"
#include "Label.h"
#include <string>
#include <memory>

namespace NexusSDK {
namespace UI {

class Tooltip : public Container {
public:
    Tooltip() : Container() {
        IgnoreLayoutCursor = true;
    }
    virtual ~Tooltip() = default;

    // Fast factory function for simple string tooltips
    static std::shared_ptr<Tooltip> Create(const std::string& text) {
        auto tooltip = std::make_shared<Tooltip>();
        auto label = std::make_shared<Label>(text);
        tooltip->AddChild(label);
        return tooltip;
    }

protected:
    virtual void OnDraw(const Rectangle& bounds) override {
        ImVec2 autoSize = GetAutoSize();
        float scale = UIScale::Get();
        
        ImGui::BeginTooltip();
        
        SpriteBatch::ClaimSpace(ImVec2(autoSize.x * scale, autoSize.y * scale));
        ImVec2 scrolledPos = ImGui::GetItemRectMin();
        
        Rectangle clientBounds;
        clientBounds.X = scrolledPos.x;
        clientBounds.Y = scrolledPos.y;
        clientBounds.Width = autoSize.x * scale;
        clientBounds.Height = autoSize.y * scale;
        
        DrawChildren(clientBounds);
        ImGui::EndTooltip();
    }
};

} // namespace UI
} // namespace NexusSDK
