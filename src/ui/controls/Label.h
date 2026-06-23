#pragma once

#include "ControlBase.h"
#include "../../utils/AsyncFont.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace NexusSDK {
namespace UI {

struct LabelPart {
    std::string Text;
    
    // Styling
    std::shared_ptr<AsyncFont> Font = nullptr;
    bool HasColor = false;
    ImVec4 TextColor = {1.0f, 1.0f, 1.0f, 1.0f};
    
    // Links & Interactions
    bool IsLink = false;
    std::string LinkID = "";
    ImVec4 HoverColor = {0.6f, 0.8f, 1.0f, 1.0f};
    
    // Decoration
    bool IsUnderlined = false;
    bool IsStrikeThrough = false;
    bool IsBold = false;
};

class Label : public ControlBase {
public:
    Label();
    Label(const std::string& markupText);
    virtual ~Label() = default;

    // Direct Part Management
    void AddPart(const LabelPart& part);
    void ClearParts();

    // Markup Parsing
    void SetMarkupText(const std::string& markupText);

    // Layout
    ImVec2 CalcSize() const;

    // Link Management
    void RegisterLink(const std::string& linkID, std::function<void()> callback);

    // Settings
    bool WrapText = true;
    std::shared_ptr<AsyncFont> Font = nullptr;

protected:
    virtual void OnDraw(const Rectangle& bounds, float scale) override;

    virtual ImVec2 GetAutoSize(float scale) const override {
        ImVec2 size = CalcSize();
        if (size.y == 0.0f) size.y = 16.0f * scale;
        return ImVec2(size.x / scale, size.y / scale);
    }

private:
    std::vector<LabelPart> m_parts;
    std::map<std::string, std::function<void()>> m_linkCallbacks;
    
    ImVec4 ParseHexColor(const std::string& hex);
};

} // namespace UI
} // namespace NexusSDK
