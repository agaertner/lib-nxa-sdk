#pragma once

#include <string>
#include <vector>
#include <imgui.h>
#include "controls/ControlBase.h"

#include "../utils/AsyncTexture.h"
#include "../utils/AsyncFont.h"

namespace NexusSDK {
namespace UI {

struct MarkupPart {
    std::string Text;
    
    // Styling
    AsyncFont* Font = nullptr;
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
    bool IsItalic = false;
    bool HasStroke = false;

    ImVec2 CalcSize(ImFont* fallbackFont) const;
};

class MarkupString {
public:
    MarkupString() = default;
    MarkupString(const std::string& markupText);
    
    void Parse(const std::string& markupText);
    void Clear();
    void AddPart(const MarkupPart& part);
    
    const std::vector<MarkupPart>& GetParts() const { return m_parts; }
    
private:
    std::vector<MarkupPart> m_parts;
    ImVec4 ParseHexColor(const std::string& hex);
};

class SpriteBatch {
public:
    // String measuring
    static ImVec2 MeasureString(const MarkupString& text, AsyncFont* defaultFont = nullptr, float wrapWidth = 0.0f);
    static ImVec2 MeasureString(const std::string& text, AsyncFont* defaultFont = nullptr, float wrapWidth = 0.0f);

    // Layout Helpers
    static void ClaimSpace(ImVec2 size);
    static void ClaimWidth(float width = 1.0f);
    static void ClaimHeight(float height = 1.0f);

    // Native text rendering. Returns the ActionID if a custom link was clicked.
    static std::string DrawString(const MarkupString& text, AsyncFont* defaultFont, ImVec2 screenPos, float wrapWidthScreen = 0.0f);
    static std::string DrawString(const std::string& text, AsyncFont* defaultFont, ImVec2 screenPos, float wrapWidthScreen = 0.0f);

    // Control-relative text rendering
    static std::string DrawStringOnCtrl(ControlBase* ctrl, const MarkupString& text, AsyncFont* defaultFont, ImVec2 localPos, float wrapWidth = 0.0f);
    static std::string DrawStringOnCtrl(ControlBase* ctrl, const std::string& text, AsyncFont* defaultFont, ImVec2 localPos, float wrapWidth = 0.0f);

    // Native texture rendering
    static void DrawTexture(AsyncTexture* tex, ImVec2 minPos, ImVec2 maxPos, ImColor color = IM_COL32_WHITE);
    static void DrawTexture(AsyncTexture* tex, ImVec2 minPos, ImVec2 maxPos, ImVec2 uvMin, ImVec2 uvMax, ImColor color = IM_COL32_WHITE);

    // Control-relative texture rendering
    static void DrawTextureOnCtrl(ControlBase* ctrl, AsyncTexture* tex, Rectangle destRect, ImColor color = IM_COL32_WHITE);
    static void DrawTextureOnCtrl(ControlBase* ctrl, AsyncTexture* tex, Rectangle destRect, Rectangle sourceRect, ImColor color = IM_COL32_WHITE);
    static void DrawTextureOnCtrl(ControlBase* ctrl, AsyncTexture* tex, ImVec2 localPos, ImColor color = IM_COL32_WHITE);
    
    // Native primitive rendering
    static void DrawRect(ImVec2 minPos, ImVec2 maxPos, ImColor color, float thickness = 1.0f, float rounding = 0.0f);
    static void DrawFilledRect(ImVec2 minPos, ImVec2 maxPos, ImColor color, float rounding = 0.0f);
    static void DrawLine(ImVec2 startScreen, ImVec2 endScreen, ImColor color, float thickness = 1.0f);
    static void DrawTriangleFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color);

    // Control-relative primitive rendering
    static void DrawRectOnCtrl(ControlBase* ctrl, Rectangle destRect, ImColor color, float thickness = 1.0f, float rounding = 0.0f);
    static void DrawFilledRectOnCtrl(ControlBase* ctrl, Rectangle destRect, ImColor color, float rounding = 0.0f);
    static void DrawLineOnCtrl(ControlBase* ctrl, ImVec2 start, ImVec2 end, ImColor color, float thickness = 1.0f);
    static void DrawTriangleFilledOnCtrl(ControlBase* ctrl, ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color);
};

} // namespace UI
} // namespace NexusSDK
