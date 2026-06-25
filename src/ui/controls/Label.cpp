#include "Label.h"
#include "../../NexusSDK.h"
#include <windows.h>
#include <sstream>

namespace NexusSDK {
namespace UI {

Label::Label() : ControlBase() {}

Label::Label(const std::string& markupText) : ControlBase() {
    SetMarkupText(markupText);
}

void Label::AddPart(const LabelPart& part) {
    m_parts.push_back(part);
}

void Label::ClearParts() {
    m_parts.clear();
}

void Label::RegisterAction(const std::string& actionID, std::function<void()> callback) {
    m_actionCallbacks[actionID] = callback;
}

ImVec4 Label::ParseHexColor(const std::string& hex) {
    if (hex.length() != 6) return {1, 1, 1, 1};
    int r, g, b;
    if (sscanf_s(hex.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
        return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
    }
    return {1, 1, 1, 1};
}

void Label::SetMarkupText(const std::string& markupText) {
    ClearParts();

    bool hasColor = false;
    ImVec4 currentColor = {1, 1, 1, 1};
    bool isLink = false;
    std::string currentActionID = "";
    std::string currentHref = "";
    bool isBold = false;
    bool isStrike = false;
    bool isUnderline = false;
    bool isItalic = false;
    bool isStroke = false;
    std::string currentFont = "";
    float currentSize = 0.0f;
    
    std::string buffer = "";

    auto pushPart = [&]() {
        if (!buffer.empty()) {
            LabelPart part;
            part.Text = buffer;
            part.HasColor = hasColor;
            part.TextColor = currentColor;
            part.IsLink = isLink;
            part.ActionID = currentActionID;
            part.HrefURL = currentHref;
            part.IsBold = isBold;
            part.IsStrikeThrough = isStrike;
            part.IsUnderlined = isUnderline || isLink;
            part.IsItalic = isItalic;
            part.HasStroke = isStroke;
            
            if (!currentFont.empty() || currentSize > 0.0f) {
                part.FontId = currentFont.empty() ? "NXA_FONT_MENOMONIA" : currentFont;
                part.FontSize = currentSize > 0.0f ? currentSize : 16.0f;
            }
            
            AddPart(part);
            buffer.clear();
        }
    };

    size_t i = 0;
    while (i < markupText.length()) {
        if (markupText[i] == '<') {
            size_t endTag = markupText.find('>', i);
            if (endTag != std::string::npos) {
                std::string tag = markupText.substr(i + 1, endTag - i - 1);
                
                if (tag.rfind("c=#", 0) == 0 && tag.length() == 9) { // <c=#RRGGBB>
                    pushPart();
                    hasColor = true;
                    currentColor = ParseHexColor(tag.substr(3));
                    i = endTag + 1;
                    continue;
                } else if (tag == "/c") {
                    pushPart();
                    hasColor = false;
                    i = endTag + 1;
                    continue;
                } else if (tag.rfind("cref=", 0) == 0) { // <cref=ID>
                    pushPart();
                    isLink = true;
                    std::string actionVal = tag.substr(7);
                    if (actionVal.length() >= 2 && actionVal.front() == '"' && actionVal.back() == '"') {
                        actionVal = actionVal.substr(1, actionVal.length() - 2);
                    }
                    currentActionID = actionVal;
                    i = endTag + 1;
                    continue;
                } else if (tag == "/cref") {
                    pushPart();
                    isLink = false;
                    currentActionID = "";
                    i = endTag + 1;
                    continue;
                } else if (tag.rfind("href=", 0) == 0) { // <href=URL>
                    pushPart();
                    isLink = true;
                    std::string linkVal = tag.substr(5);
                    if (linkVal.length() >= 2 && linkVal.front() == '"' && linkVal.back() == '"') {
                        linkVal = linkVal.substr(1, linkVal.length() - 2);
                    }
                    currentHref = linkVal;
                    i = endTag + 1;
                    continue;
                } else if (tag == "/href") {
                    pushPart();
                    isLink = false;
                    currentHref = "";
                    i = endTag + 1;
                    continue;
                } else if (tag == "b") {
                    pushPart();
                    isBold = true;
                    i = endTag + 1;
                    continue;
                } else if (tag == "/b") {
                    pushPart();
                    isBold = false;
                    i = endTag + 1;
                    continue;
                } else if (tag == "s") {
                    pushPart();
                    isStrike = true;
                    i = endTag + 1;
                    continue;
                } else if (tag == "/s") {
                    pushPart();
                    isStrike = false;
                    i = endTag + 1;
                    continue;
                } else if (tag == "u") {
                    pushPart();
                    isUnderline = true;
                    i = endTag + 1;
                    continue;
                } else if (tag == "/u") {
                    pushPart();
                    isUnderline = false;
                    i = endTag + 1;
                    continue;
                } else if (tag == "i") {
                    pushPart();
                    isItalic = true;
                    i = endTag + 1;
                    continue;
                } else if (tag == "/i") {
                    pushPart();
                    isItalic = false;
                    i = endTag + 1;
                    continue;
                } else if (tag == "stroke") {
                    pushPart();
                    isStroke = true;
                    i = endTag + 1;
                    continue;
                } else if (tag == "/stroke") {
                    pushPart();
                    isStroke = false;
                    i = endTag + 1;
                    continue;
                } else if (tag.rfind("font=\"", 0) == 0 && tag.back() == '"') { // <font="ID">
                    pushPart();
                    currentFont = tag.substr(6, tag.length() - 7);
                    i = endTag + 1;
                    continue;
                } else if (tag == "/font") {
                    pushPart();
                    currentFont = "";
                    i = endTag + 1;
                    continue;
                } else if (tag.rfind("size=", 0) == 0) { // <size=42>
                    pushPart();
                    try {
                        currentSize = std::stof(tag.substr(5));
                    } catch(...) {
                        currentSize = 0.0f;
                    }
                    i = endTag + 1;
                    continue;
                } else if (tag == "/size") {
                    pushPart();
                    currentSize = 0.0f;
                    i = endTag + 1;
                    continue;
                }
            }
        }
        buffer += markupText[i];
        i++;
    }
    pushPart();
}

ImVec2 LabelPart::CalcSize(ImFont* fallbackFont) const {
    ImFont* activeFont = this->Font;
    if (!this->FontId.empty() || this->FontSize > 0.0f) {
        std::string fontId = this->FontId.empty() ? "NXA_FONT_MENOMONIA" : this->FontId;
        float size = this->FontSize > 0.0f ? this->FontSize : 16.0f;
        if (NexusSDK::Content) {
            activeFont = NexusSDK::Content->GetFont(fontId, size);
        }
    }
    struct Font fontGuard(activeFont ? activeFont : fallbackFont);
    return ImGui::CalcTextSize(this->Text.c_str());
}

ImVec2 Label::CalcSize() const {
    float width = 0.0f;
    float height = 0.0f;
    
    // Quick approximation for non-wrapped text
    for (const auto& part : m_parts) {
        ImVec2 partSize = part.CalcSize(this->Font);
        width += partSize.x;
        height = (std::max)(height, partSize.y);
    }
    return ImVec2(width, height);
}

void Label::OnDraw(const Rectangle& bounds, float scale) {
    ImGui::SetCursorScreenPos(bounds.GetMin());
    float startXScreen = bounds.X;
    float maxWidth = bounds.Width > 0 ? bounds.Width : 99999.0f;
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    bool isFirstTokenOnLine = true;

    for (size_t i = 0; i < m_parts.size(); ++i) {
        auto& part = m_parts[i];
        
        ImFont* activeFont = part.Font;
        if (!part.FontId.empty() || part.FontSize > 0.0f) {
            std::string fontId = part.FontId.empty() ? "NXA_FONT_MENOMONIA" : part.FontId;
            float size = part.FontSize > 0.0f ? part.FontSize : 16.0f;
            if (NexusSDK::Content) {
                activeFont = NexusSDK::Content->GetFont(fontId, size);
            }
        }
        struct Font fontGuard(activeFont ? activeFont : this->Font);

        // Tokenize by space and newline to allow wrapping
        std::vector<std::string> tokens;
        std::string currentToken = "";
        for (char c : part.Text) {
            if (c == ' ' || c == '\n') {
                if (!currentToken.empty()) tokens.push_back(currentToken);
                tokens.push_back(std::string(1, c));
                currentToken = "";
            } else {
                currentToken += c;
            }
        }
        if (!currentToken.empty()) tokens.push_back(currentToken);

        ImVec4 normalColor = part.HasColor ? part.TextColor : ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImU32 normalColU32 = ImGui::ColorConvertFloat4ToU32(normalColor);
        ImU32 hoverColU32 = ImGui::ColorConvertFloat4ToU32(part.HoverColor);

        int part_vtx_start = ImGui::GetWindowDrawList()->VtxBuffer.Size;
        bool isPartHovered = false;

        for (const auto& token : tokens) {
            if (token == "\n") {
                ImGui::NewLine();
                ImGui::SetCursorScreenPos(ImVec2(startXScreen, ImGui::GetCursorScreenPos().y));
                isFirstTokenOnLine = true;
                continue;
            }

            if (!isFirstTokenOnLine) {
                ImGui::SameLine(0.0f, 0.0f);
            }

            float currentXScreen = ImGui::GetCursorScreenPos().x;
            ImVec2 size = ImGui::CalcTextSize(token.c_str());
            
            // Text Wrapping Logic
            if (WrapText && !isFirstTokenOnLine && (currentXScreen + size.x > startXScreen + maxWidth)) {
                ImGui::NewLine();
                ImGui::SetCursorScreenPos(ImVec2(startXScreen, ImGui::GetCursorScreenPos().y));
                isFirstTokenOnLine = true;
                if (token == " ") continue; // Skip leading space on a new line
                currentXScreen = ImGui::GetCursorScreenPos().x; // Update for the new line
            }

            int vtx_idx_start = ImGui::GetWindowDrawList()->VtxBuffer.Size;

            // Draw the text
            if (part.HasStroke) {
                ImVec2 p = ImGui::GetCursorScreenPos();
                ImU32 strokeCol = IM_COL32(0, 0, 0, 255); // black border
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImFont* font = ImGui::GetFont();
                float font_size = ImGui::GetFontSize();
                drawList->AddText(font, font_size, ImVec2(p.x - 1.0f, p.y), strokeCol, token.c_str());
                drawList->AddText(font, font_size, ImVec2(p.x + 1.0f, p.y), strokeCol, token.c_str());
                drawList->AddText(font, font_size, ImVec2(p.x, p.y - 1.0f), strokeCol, token.c_str());
                drawList->AddText(font, font_size, ImVec2(p.x, p.y + 1.0f), strokeCol, token.c_str());
                drawList->AddText(font, font_size, ImVec2(p.x - 1.0f, p.y - 1.0f), strokeCol, token.c_str());
                drawList->AddText(font, font_size, ImVec2(p.x + 1.0f, p.y - 1.0f), strokeCol, token.c_str());
                drawList->AddText(font, font_size, ImVec2(p.x - 1.0f, p.y + 1.0f), strokeCol, token.c_str());
                drawList->AddText(font, font_size, ImVec2(p.x + 1.0f, p.y + 1.0f), strokeCol, token.c_str());
            }

            if (part.IsBold) {
                ImVec2 p = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(p.x + 1.0f, p.y), normalColU32, token.c_str());
            }

            if (part.HasColor || part.IsLink) {
                ImGui::TextColored(normalColor, "%s", token.c_str());
            } else {
                ImGui::Text("%s", token.c_str());
            }

            ImVec2 minPos = ImGui::GetItemRectMin();
            ImVec2 maxPos = ImGui::GetItemRectMax();
            ImVec2 itemSize = ImGui::GetItemRectSize();
            
            // Adjust hover bounds slightly downwards to better match visual font baselines
            ImVec2 hoverMin = ImVec2(minPos.x, minPos.y + (itemSize.y * 0.15f));
            ImVec2 hoverMax = ImVec2(maxPos.x, maxPos.y + (itemSize.y * 0.15f));
            
            if (part.IsLink && ImGui::IsMouseHoveringRect(hoverMin, hoverMax, true)) {
                isPartHovered = true;
            }

            int vtx_idx_end = ImGui::GetWindowDrawList()->VtxBuffer.Size;

            if (part.IsItalic) {
                float f_size = ImGui::GetFontSize();
                for (int vtx = vtx_idx_start; vtx < vtx_idx_end; ++vtx) {
                    ImDrawVert& v = ImGui::GetWindowDrawList()->VtxBuffer[vtx];
                    v.pos.x += (v.pos.y - (minPos.y + f_size)) * -0.22f;
                }
            }

            isFirstTokenOnLine = false;

            // Draw Decorations
            if (part.IsUnderlined || part.IsLink) {
                ImGui::GetWindowDrawList()->AddLine(ImVec2(minPos.x, maxPos.y), ImVec2(maxPos.x, maxPos.y), normalColU32, 2.0f);
            }
            if (part.IsStrikeThrough) {
                // Shift down slightly because lowercase letters sit lower in the vertical space
                float midY = minPos.y + (size.y * 0.55f);
                ImGui::GetWindowDrawList()->AddLine(ImVec2(minPos.x, midY), ImVec2(maxPos.x, midY), normalColU32, 2.0f);
            }
        }
        
        if (isPartHovered) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (ImGui::IsMouseClicked(0)) {
                if (!part.HrefURL.empty()) {
                    ShellExecuteA(0, 0, part.HrefURL.c_str(), 0, 0, SW_SHOW);
                } else if (!part.ActionID.empty() && m_actionCallbacks.count(part.ActionID)) {
                    m_actionCallbacks[part.ActionID]();
                }
            }
            
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            for (int vtx = part_vtx_start; vtx < drawList->VtxBuffer.Size; ++vtx) {
                if (drawList->VtxBuffer[vtx].col == normalColU32) {
                    drawList->VtxBuffer[vtx].col = hoverColU32;
                }
            }
        }
    }
}

} // namespace UI
} // namespace NexusSDK
