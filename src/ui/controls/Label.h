#pragma once

#include "ControlBase.h"
#include "../../utils/ImStateGuards.h"
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
    ImFont* Font = nullptr;
    std::string FontId = "";
    float FontSize = 0.0f;
    bool HasColor = false;
    ImVec4 TextColor = {1.0f, 1.0f, 1.0f, 1.0f};
    
    // Links & Interactions
    bool IsLink = false;
    std::string ActionID = "";
    std::string HrefURL = "";
    ImVec4 HoverColor = {0.6f, 0.8f, 1.0f, 1.0f};
    
    // Decoration
    bool IsUnderlined = false;
    bool IsStrikeThrough = false;
    bool IsBold = false;
    bool HasStroke = false;

    ImVec2 CalcSize(ImFont* fallbackFont) const;
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
    void RegisterAction(const std::string& actionID, std::function<void()> callback);

    // Settings
    bool WrapText = true;

protected:
    virtual void OnDraw(const Rectangle& bounds, float scale) override;

    virtual ImVec2 GetAutoSize(float scale) const override {
        ImVec2 size = CalcSize();
        if (size.y == 0.0f) size.y = 16.0f * scale;
        return ImVec2(size.x / scale, size.y / scale);
    }

private:
    std::vector<LabelPart> m_parts;
    std::map<std::string, std::function<void()>> m_actionCallbacks;
    
    ImVec4 ParseHexColor(const std::string& hex);
};

} // namespace UI
} // namespace NexusSDK
