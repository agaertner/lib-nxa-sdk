#include "SpriteBatch.h"
#include "UIScale.h"
#include "../NexusSDK.h"
#include "../utils/ImStateGuards.h"
#include <windows.h>
#include <algorithm>
#include "../utils/AsyncTexture.h"

namespace NexusSDK {
namespace UI {

ImVec2 MarkupPart::CalcSize(ImFont* fallbackFont) const {
    ImFont* activeFont = this->Font ? this->Font->Get() : nullptr;
    if (!this->FontId.empty() || this->FontSize > 0.0f) {
        std::string fontId = this->FontId.empty() ? "NXA_FONT_MENOMONIA" : this->FontId;
        float size = this->FontSize > 0.0f ? this->FontSize : 16.0f;
        if (NexusSDK::Content) {
            AsyncFont* aFont = NexusSDK::Content->GetFont(fontId, size);
            activeFont = aFont ? aFont->Get() : nullptr;
        }
    }
    FontGuard font(activeFont ? activeFont : fallbackFont);
    return ImGui::CalcTextSize(this->Text.c_str());
}

MarkupString::MarkupString(const std::string& markupText) {
    Parse(markupText);
}

void MarkupString::AddPart(const MarkupPart& part) {
    m_parts.push_back(part);
}

void MarkupString::Clear() {
    m_parts.clear();
}

ImVec4 MarkupString::ParseHexColor(const std::string& hex) {
    if (hex.length() != 6) return {1, 1, 1, 1};
    int r, g, b;
    if (sscanf_s(hex.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
        return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
    }
    return {1, 1, 1, 1};
}

void MarkupString::Parse(const std::string& markupText) {
    Clear();

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
            MarkupPart part;
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

ImVec2 SpriteBatch::MeasureString(const std::string& text, AsyncFont* defaultFont, float wrapWidth) {
    MarkupString markup(text);
    return MeasureString(markup, defaultFont, wrapWidth);
}

ImVec2 SpriteBatch::MeasureString(const MarkupString& text, AsyncFont* defaultFont, float wrapWidth) {
    float maxWidth = 0.0f;
    float currentX = 0.0f;
    float currentY = 0.0f;
    float maxLineHeight = 0.0f;

    bool isFirstTokenOnLine = true;
    float scale = UIScale::Get();

    for (const auto& part : text.GetParts()) {
        ImFont* activeFont = part.Font ? part.Font->Get() : nullptr;
        if (!part.FontId.empty() || part.FontSize > 0.0f) {
            std::string fontId = part.FontId.empty() ? "NXA_FONT_MENOMONIA" : part.FontId;
            float size = part.FontSize > 0.0f ? part.FontSize : 16.0f;
            if (NexusSDK::Content) {
                AsyncFont* aFont = NexusSDK::Content->GetFont(fontId, size);
                activeFont = aFont ? aFont->Get() : nullptr;
            }
        }
        ImFont* fontToUse = activeFont ? activeFont : (defaultFont ? defaultFont->Get() : ImGui::GetFont());
        FontGuard font(fontToUse);

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
                maxWidth = (std::max)(maxWidth, currentX);
                currentX = 0.0f;
                currentY += (maxLineHeight > 0.0f ? maxLineHeight : fontToUse->FontSize);
                maxLineHeight = 0.0f;
                isFirstTokenOnLine = true;
                continue;
            }

            ImVec2 size = ImGui::CalcTextSize(token.c_str());

            if (wrapWidth > 0.0f && !isFirstTokenOnLine && ((currentX + size.x) > wrapWidth)) {
                maxWidth = (std::max)(maxWidth, currentX);
                currentX = 0.0f;
                currentY += (maxLineHeight > 0.0f ? maxLineHeight : fontToUse->FontSize);
                maxLineHeight = 0.0f;
                isFirstTokenOnLine = true;
                if (token == " ") continue;
            }

            currentX += size.x;
            maxLineHeight = (std::max)(maxLineHeight, size.y);
            isFirstTokenOnLine = false;
        }
    }
    
    maxWidth = (std::max)(maxWidth, currentX);
    if (currentX > 0 || currentY == 0) {
        currentY += maxLineHeight;
    }

    return ImVec2(maxWidth, currentY);
}

std::string SpriteBatch::DrawString(const std::string& text, AsyncFont* defaultFont, ImVec2 screenPos, float wrapWidthScreen) {
    MarkupString markup(text);
    return DrawString(markup, defaultFont, screenPos, wrapWidthScreen);
}

std::string SpriteBatch::DrawString(const MarkupString& text, AsyncFont* defaultFont, ImVec2 screenPos, float wrapWidthScreen) {
    float startXScreen = screenPos.x;
    float currentXScreen = startXScreen;
    float currentYScreen = screenPos.y;
    float maxLineHeightScreen = 0.0f;

    bool isFirstTokenOnLine = true;
    std::string clickedAction = "";
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 mousePos = ImGui::GetMousePos();
    bool isMouseClicked = ImGui::IsMouseClicked(0);

    for (const auto& part : text.GetParts()) {
        ImFont* activeFont = part.Font ? part.Font->Get() : nullptr;
        if (!part.FontId.empty() || part.FontSize > 0.0f) {
            std::string fontId = part.FontId.empty() ? "NXA_FONT_MENOMONIA" : part.FontId;
            float size = part.FontSize > 0.0f ? part.FontSize : 16.0f;
            if (NexusSDK::Content) {
                AsyncFont* aFont = NexusSDK::Content->GetFont(fontId, size);
                activeFont = aFont ? aFont->Get() : nullptr;
            }
        }
        ImFont* fontToUse = activeFont ? activeFont : (defaultFont ? defaultFont->Get() : ImGui::GetFont());
        FontGuard font(fontToUse);

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
        
        int part_vtx_start = drawList->VtxBuffer.Size;
        bool isPartHovered = false;

        for (const auto& token : tokens) {
            if (token == "\n") {
                currentXScreen = startXScreen;
                currentYScreen += (maxLineHeightScreen > 0.0f ? maxLineHeightScreen : fontToUse->FontSize);
                maxLineHeightScreen = 0.0f;
                isFirstTokenOnLine = true;
                continue;
            }

            ImVec2 size = ImGui::CalcTextSize(token.c_str());

            if (wrapWidthScreen > 0.0f && !isFirstTokenOnLine && ((currentXScreen - startXScreen) + size.x > wrapWidthScreen)) {
                currentXScreen = startXScreen;
                currentYScreen += (maxLineHeightScreen > 0.0f ? maxLineHeightScreen : fontToUse->FontSize);
                maxLineHeightScreen = 0.0f;
                isFirstTokenOnLine = true;
                if (token == " ") continue;
            }

            ImVec2 pos(currentXScreen, currentYScreen);
            int vtx_idx_start = drawList->VtxBuffer.Size;

            if (token != " ") {
                if (part.HasStroke) {
                    ImU32 strokeCol = IM_COL32(0, 0, 0, 255);
                    float font_size = fontToUse->FontSize;
                    drawList->AddText(fontToUse, font_size, ImVec2(pos.x - 1.0f, pos.y), strokeCol, token.c_str());
                    drawList->AddText(fontToUse, font_size, ImVec2(pos.x + 1.0f, pos.y), strokeCol, token.c_str());
                    drawList->AddText(fontToUse, font_size, ImVec2(pos.x, pos.y - 1.0f), strokeCol, token.c_str());
                    drawList->AddText(fontToUse, font_size, ImVec2(pos.x, pos.y + 1.0f), strokeCol, token.c_str());
                    drawList->AddText(fontToUse, font_size, ImVec2(pos.x - 1.0f, pos.y - 1.0f), strokeCol, token.c_str());
                    drawList->AddText(fontToUse, font_size, ImVec2(pos.x + 1.0f, pos.y - 1.0f), strokeCol, token.c_str());
                    drawList->AddText(fontToUse, font_size, ImVec2(pos.x - 1.0f, pos.y + 1.0f), strokeCol, token.c_str());
                    drawList->AddText(fontToUse, font_size, ImVec2(pos.x + 1.0f, pos.y + 1.0f), strokeCol, token.c_str());
                }

                if (part.IsBold) {
                    drawList->AddText(fontToUse, fontToUse->FontSize, ImVec2(pos.x + 1.0f, pos.y), normalColU32, token.c_str());
                }

                drawList->AddText(fontToUse, fontToUse->FontSize, pos, normalColU32, token.c_str());
            }

            ImVec2 minPos = pos;
            ImVec2 maxPos = ImVec2(pos.x + size.x, pos.y + size.y);
            
            ImVec2 hoverMin = ImVec2(minPos.x, minPos.y + (size.y * 0.15f));
            ImVec2 hoverMax = ImVec2(maxPos.x, maxPos.y + (size.y * 0.15f));
            
            if (part.IsLink && token != " " && token != "\n") {
                if (mousePos.x >= hoverMin.x && mousePos.x <= hoverMax.x && 
                    mousePos.y >= hoverMin.y && mousePos.y <= hoverMax.y) {
                    isPartHovered = true;
                }
            }

            int vtx_idx_end = drawList->VtxBuffer.Size;

            if (part.IsItalic && token != " ") {
                float f_size = fontToUse->FontSize;
                for (int vtx = vtx_idx_start; vtx < vtx_idx_end; ++vtx) {
                    ImDrawVert& v = drawList->VtxBuffer[vtx];
                    v.pos.x += (v.pos.y - (minPos.y + f_size)) * -0.22f;
                }
            }

            if (token != " " && token != "\n") {
                if (part.IsUnderlined || part.IsLink) {
                    drawList->AddLine(ImVec2(minPos.x, maxPos.y), ImVec2(maxPos.x, maxPos.y), normalColU32, 2.0f);
                }
                if (part.IsStrikeThrough) {
                    float midY = minPos.y + (size.y * 0.55f);
                    drawList->AddLine(ImVec2(minPos.x, midY), ImVec2(maxPos.x, midY), normalColU32, 2.0f);
                }
            }

            currentXScreen += size.x;
            maxLineHeightScreen = (std::max)(maxLineHeightScreen, size.y);
            isFirstTokenOnLine = false;
        }

        if (isPartHovered) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (isMouseClicked) {
                if (!part.HrefURL.empty()) {
                    ShellExecuteA(0, 0, part.HrefURL.c_str(), 0, 0, SW_SHOW);
                } else if (!part.ActionID.empty()) {
                    clickedAction = part.ActionID;
                }
            }
            
            for (int vtx = part_vtx_start; vtx < drawList->VtxBuffer.Size; ++vtx) {
                if (drawList->VtxBuffer[vtx].col == normalColU32) {
                    drawList->VtxBuffer[vtx].col = hoverColU32;
                }
            }
        }
    }
    
    return clickedAction;
}

std::string SpriteBatch::DrawStringOnCtrl(ControlBase* ctrl, const std::string& text, AsyncFont* defaultFont, ImVec2 localPos, float wrapWidth) {
    MarkupString markup(text);
    return DrawStringOnCtrl(ctrl, markup, defaultFont, localPos, wrapWidth);
}

std::string SpriteBatch::DrawStringOnCtrl(ControlBase* ctrl, const MarkupString& text, AsyncFont* defaultFont, ImVec2 localPos, float wrapWidth) {
    if (!ctrl) return "";

    Rectangle bounds = ctrl->GetAbsoluteBounds().ToBounds( {localPos.x, localPos.y, 0, 0});
    float wrapWidthScreen = wrapWidth * UIScale::Get();

    return DrawString(text, defaultFont, bounds.GetMin(), wrapWidthScreen);
}

void SpriteBatch::ClaimSpace(ImVec2 size) {
    ImGui::Dummy(size);
}

void SpriteBatch::ClaimWidth(float width) {
    ImGui::Dummy(ImVec2(width, 0.0f));
}

void SpriteBatch::ClaimHeight(float height) {
    ImGui::Dummy(ImVec2(0.0f, height));
}

void SpriteBatch::DrawTexture(AsyncTexture* texAsync, ImVec2 minPos, ImVec2 maxPos, ImColor color) {
    if (!texAsync) return;
    Texture_t* tex = texAsync->Get();
    if (!tex) return;
    ImGui::GetWindowDrawList()->AddImage((ImTextureID)tex->Resource, minPos, maxPos, ImVec2(0, 0), ImVec2(1, 1), color);
}

void SpriteBatch::DrawTexture(AsyncTexture* texAsync, ImVec2 minPos, ImVec2 maxPos, ImVec2 uvMin, ImVec2 uvMax, ImColor color) {
    if (!texAsync) return;
    Texture_t* tex = texAsync->Get();
    if (!tex) return;
    ImGui::GetWindowDrawList()->AddImage((ImTextureID)tex->Resource, minPos, maxPos, uvMin, uvMax, color);
}

void SpriteBatch::DrawTextureOnCtrl(ControlBase* ctrl, AsyncTexture* texAsync, Rectangle destRect, ImColor color) {
    if (!ctrl || !texAsync) return;
    Rectangle bounds = ctrl->GetAbsoluteBounds().ToBounds(destRect);
    DrawTexture(texAsync, bounds.GetMin(), bounds.GetMax(), color);
}

void SpriteBatch::DrawTextureOnCtrl(ControlBase* ctrl, AsyncTexture* texAsync, Rectangle destRect, Rectangle sourceRect, ImColor color) {
    if (!ctrl || !texAsync) return;
    Texture_t* tex = texAsync->Get();
    if (!tex) return;
    Rectangle bounds = ctrl->GetAbsoluteBounds().ToBounds(destRect);
    
    ImVec2 uvMin(sourceRect.X / tex->Width, sourceRect.Y / tex->Height);
    ImVec2 uvMax((sourceRect.X + sourceRect.Width) / tex->Width, (sourceRect.Y + sourceRect.Height) / tex->Height);
    
    DrawTexture(texAsync, bounds.GetMin(), bounds.GetMax(), uvMin, uvMax, color);
}

void SpriteBatch::DrawTextureOnCtrl(ControlBase* ctrl, AsyncTexture* texAsync, ImVec2 localPos, ImColor color) {
    if (!ctrl || !texAsync) return;
    Texture_t* tex = texAsync->Get();
    if (!tex) return;
    Rectangle localRect = { localPos.x, localPos.y, (float)tex->Width, (float)tex->Height };
    Rectangle bounds = ctrl->GetAbsoluteBounds().ToBounds(localRect);
    
    DrawTexture(texAsync, bounds.GetMin(), bounds.GetMax(), color);
}

void SpriteBatch::DrawRect(ImVec2 minPos, ImVec2 maxPos, ImColor color, float thickness, float rounding) {
    ImGui::GetWindowDrawList()->AddRect(minPos, maxPos, color, rounding, 0, thickness);
}

void SpriteBatch::DrawFilledRect(ImVec2 minPos, ImVec2 maxPos, ImColor color, float rounding) {
    ImGui::GetWindowDrawList()->AddRectFilled(minPos, maxPos, color, rounding);
}

void SpriteBatch::DrawLine(ImVec2 startScreen, ImVec2 endScreen, ImColor color, float thickness) {
    ImGui::GetWindowDrawList()->AddLine(startScreen, endScreen, color, thickness);
}

void SpriteBatch::DrawTriangleFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color) {
    ImGui::GetWindowDrawList()->AddTriangleFilled(p1, p2, p3, color);
}

void SpriteBatch::DrawRectOnCtrl(ControlBase* ctrl, Rectangle destRect, ImColor color, float thickness, float rounding) {
    if (!ctrl) return;
    Rectangle bounds = ctrl->GetAbsoluteBounds().ToBounds( destRect);
    DrawRect(bounds.GetMin(), bounds.GetMax(), color, thickness, rounding);
}

void SpriteBatch::DrawFilledRectOnCtrl(ControlBase* ctrl, Rectangle destRect, ImColor color, float rounding) {
    if (!ctrl) return;
    Rectangle bounds = ctrl->GetAbsoluteBounds().ToBounds( destRect);
    DrawFilledRect(bounds.GetMin(), bounds.GetMax(), color, rounding);
}

void SpriteBatch::DrawLineOnCtrl(ControlBase* ctrl, ImVec2 start, ImVec2 end, ImColor color, float thickness) {
    if (!ctrl) return;
    Rectangle startRect = ctrl->GetAbsoluteBounds().ToBounds({start.x, start.y, 0, 0});
    Rectangle endRect = ctrl->GetAbsoluteBounds().ToBounds({end.x, end.y, 0, 0});
    DrawLine(startRect.GetMin(), endRect.GetMin(), color, thickness);
}

void SpriteBatch::DrawTriangleFilledOnCtrl(ControlBase* ctrl, ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color) {
    if (!ctrl) return;
    ImVec2 s1 = ctrl->GetAbsoluteBounds().ToBounds({p1.x, p1.y, 0, 0}).GetMin();
    ImVec2 s2 = ctrl->GetAbsoluteBounds().ToBounds({p2.x, p2.y, 0, 0}).GetMin();
    ImVec2 s3 = ctrl->GetAbsoluteBounds().ToBounds({p3.x, p3.y, 0, 0}).GetMin();
    DrawTriangleFilled(s1, s2, s3, color);
}

} // namespace UI
} // namespace NexusSDK
