#include "Label.h"
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

void Label::RegisterLink(const std::string& linkID, std::function<void()> callback) {
    m_linkCallbacks[linkID] = callback;
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
    std::string currentLinkID = "";
    
    std::string buffer = "";

    auto pushPart = [&]() {
        if (!buffer.empty()) {
            LabelPart part;
            part.Text = buffer;
            part.HasColor = hasColor;
            part.TextColor = currentColor;
            part.IsLink = isLink;
            part.LinkID = currentLinkID;
            if (isLink) part.IsUnderlined = true;
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
                } else if (tag.rfind("link=", 0) == 0) { // <link=ID>
                    pushPart();
                    isLink = true;
                    currentLinkID = tag.substr(5);
                    i = endTag + 1;
                    continue;
                } else if (tag == "/link") {
                    pushPart();
                    isLink = false;
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

ImVec2 Label::CalcSize() {
    float width = 0.0f;
    float height = 0.0f;
    
    // Quick approximation for non-wrapped text
    for (const auto& part : m_parts) {
        bool pushedFont = false;
        if (part.Font && part.Font->Get()) {
            ImGui::PushFont((ImFont*)part.Font->Get());
            pushedFont = true;
        } else if (Font && Font->Get()) {
            ImGui::PushFont((ImFont*)Font->Get());
            pushedFont = true;
        }

        ImVec2 partSize = ImGui::CalcTextSize(part.Text.c_str());
        width += partSize.x;
        height = (std::max)(height, partSize.y);

        if (pushedFont) {
            ImGui::PopFont();
        }
    }
    return ImVec2(width, height);
}

void Label::OnRender() {
    float startX = ImGui::GetCursorPosX();
    float maxWidth = m_size.x > 0 ? m_size.x : ImGui::GetContentRegionAvail().x;
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    bool isFirstTokenOnLine = true;

    for (size_t i = 0; i < m_parts.size(); ++i) {
        auto& part = m_parts[i];
        
        bool pushedFont = false;
        if (part.Font && part.Font->Get()) {
            ImGui::PushFont((ImFont*)part.Font->Get());
            pushedFont = true;
        } else if (Font && Font->Get()) {
            ImGui::PushFont((ImFont*)Font->Get());
            pushedFont = true;
        }

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

        for (const auto& token : tokens) {
            if (token == "\n") {
                ImGui::NewLine();
                isFirstTokenOnLine = true;
                continue;
            }

            ImVec2 size = ImGui::CalcTextSize(token.c_str());
            float currentX = ImGui::GetCursorPosX();
            
            // Text Wrapping Logic
            if (WrapText && !isFirstTokenOnLine && (currentX + size.x > startX + maxWidth)) {
                ImGui::NewLine();
                isFirstTokenOnLine = true;
                if (token == " ") continue; // Skip leading space on a new line
            } else if (!isFirstTokenOnLine) {
                ImGui::SameLine(0.0f, 0.0f);
            }

            ImVec2 minPos = ImGui::GetCursorScreenPos();
            ImVec2 maxPos = ImVec2(minPos.x + size.x, minPos.y + size.y);
            
            bool isHovered = ImGui::IsMouseHoveringRect(minPos, maxPos, true);

            // Click handling
            if (isHovered && part.IsLink && ImGui::IsMouseClicked(0)) {
                if (m_linkCallbacks.count(part.LinkID)) {
                    m_linkCallbacks[part.LinkID]();
                }
            }

            // Colors & Hover state
            ImVec4 renderColor = part.TextColor;
            if (isHovered && part.IsLink) {
                renderColor = part.HoverColor;
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }

            // Draw the text
            if (part.HasColor || part.IsLink) {
                ImGui::TextColored(renderColor, "%s", token.c_str());
            } else {
                ImGui::Text("%s", token.c_str());
            }

            isFirstTokenOnLine = false;

            // Draw Decorations
            if (token != " ") {
                if (part.IsUnderlined || part.IsLink) {
                    drawList->AddLine(ImVec2(minPos.x, maxPos.y), ImVec2(maxPos.x, maxPos.y), ImGui::ColorConvertFloat4ToU32(renderColor));
                }
                if (part.IsStrikeThrough) {
                    float midY = minPos.y + (size.y / 2.0f);
                    drawList->AddLine(ImVec2(minPos.x, midY), ImVec2(maxPos.x, midY), ImGui::ColorConvertFloat4ToU32(renderColor));
                }
            }
        }

        if (pushedFont) {
            ImGui::PopFont();
        }
    }
}

} // namespace UI
} // namespace NexusSDK
