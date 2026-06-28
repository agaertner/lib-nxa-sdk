#include "../../nexus-sdk-resource.h"
#include "Slider.h"
#include <NexusSDK.h>
#include "../SpriteBatch.h"

#include <imgui.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace NexusSDK {
namespace UI {

    void Slider::OnDraw(const Rectangle& bounds) {
        if (!Value) return;
        float scale = UIScale::Get();

        float unscaledHeight = 24.0f;
        float height = unscaledHeight * scale;
        ImVec2 pos = bounds.GetMin();
        float width = bounds.Width > 0.0f ? bounds.Width : 200.0f * scale;
        float unscaledWidth = bounds.Width > 0.0f ? (bounds.Width / scale) : 200.0f;

        if (TextLabel) {
            float unscaledSpacing = 8.0f;
            ImVec2 textSize = TextLabel->CalcSize();
            float unscaledOffset = (unscaledHeight - textSize.y) / 2.0f;
            
            Rectangle textBounds;
            textBounds.X = pos.x;
            textBounds.Y = pos.y + (unscaledOffset * scale);
            textBounds.Width = LabelWidth > 0.0f ? LabelWidth * scale : textSize.x * scale;
            textBounds.Height = textSize.y * scale;
            
            TextLabel->Draw(textBounds);
            
            float labelSpace = LabelWidth > 0.0f ? LabelWidth * scale : (textSize.x + unscaledSpacing) * scale;
            pos.x += labelSpace;
            width -= labelSpace;
            unscaledWidth -= (labelSpace / scale);
        }

        ImGui::SetCursorScreenPos(pos);

        float unscaledNubSize = 20.0f;
        float nubSize = unscaledNubSize * scale;
        float unscaledCapW = 4.0f;
        float unscaledBoundW = 3.0f;
        float boundW = unscaledBoundW * scale;
        float usableWidth = width - (2.0f * boundW) - nubSize;
        float unscaledUsableWidth = unscaledWidth - (2.0f * unscaledBoundW) - unscaledNubSize;

        ImGui::InvisibleButton(m_id.c_str(), ImVec2(width, height));
        bool isActive = ImGui::IsItemActive();
        
        if (isActive) {
            float mousePos = ImGui::GetIO().MousePos.x - pos.x;
            float normalized = std::clamp((mousePos - boundW - nubSize / 2.0f) / usableWidth, 0.0f, 1.0f);
            float newValue = Min + (Max - Min) * normalized;
            
            if (*Value != newValue) {
                *Value = newValue;
                if (OnValueChanged) OnValueChanged(*Value);
            }
        }

        float localX = (pos.x - bounds.GetMin().x) / scale;
        float localY = 0.0f;

        // Draw track
        AsyncTexture* texTrackAsync = NexusSDK::Content->GetTexture(IDB_TRACKBAR_ATLAS_BG);
        if (texTrackAsync && texTrackAsync->GetWidth() > 0) {
            float unscaledTrackHeight = 16.0f;
            float tPosY = localY + (unscaledHeight - unscaledTrackHeight) / 2.0f;
            float texW = (float)texTrackAsync->GetWidth();
            float texH = (float)texTrackAsync->GetHeight();
            
            // Left cap
            SpriteBatch::DrawTextureOnCtrl(this, texTrackAsync, 
                Rectangle{localX, tPosY, unscaledCapW, unscaledTrackHeight}, 
                Rectangle{0, 0, (4.0f / 256.0f) * texW, texH}, IM_COL32_WHITE);
                
            // Middle
            SpriteBatch::DrawTextureOnCtrl(this, texTrackAsync, 
                Rectangle{localX + unscaledCapW, tPosY, unscaledWidth - (2*unscaledCapW), unscaledTrackHeight}, 
                Rectangle{(4.0f / 256.0f) * texW, 0, ((256.0f - 8.0f) / 256.0f) * texW, texH}, IM_COL32_WHITE);
                
            // Right cap
            SpriteBatch::DrawTextureOnCtrl(this, texTrackAsync, 
                Rectangle{localX + unscaledWidth - unscaledCapW, tPosY, unscaledCapW, unscaledTrackHeight}, 
                Rectangle{((256.0f - 4.0f) / 256.0f) * texW, 0, (4.0f / 256.0f) * texW, texH}, IM_COL32_WHITE);
        } else {
            SpriteBatch::DrawFilledRectOnCtrl(this, Rectangle{localX, localY, unscaledWidth, unscaledHeight}, IM_COL32(50,50,50,255));
        }

        // Draw nub
        AsyncTexture* texNubAsync = NexusSDK::Content->GetTexture(IDB_TRACKBAR_NUB);
        float percent = std::clamp((*Value - Min) / (Max - Min), 0.0f, 1.0f);
        float unscaledNubCenter = localX + unscaledBoundW + (unscaledNubSize / 2.0f) + (unscaledUsableWidth * percent);
        float nubMinX = unscaledNubCenter - unscaledNubSize / 2.0f;
        float nubMinY = localY + (unscaledHeight - unscaledNubSize) / 2.0f;

        if (texNubAsync && texNubAsync->GetWidth() > 0) {
            SpriteBatch::DrawTextureOnCtrl(this, texNubAsync, Rectangle{nubMinX, nubMinY, unscaledNubSize, unscaledNubSize});
        } else {
            SpriteBatch::DrawFilledRectOnCtrl(this, Rectangle{nubMinX, nubMinY, unscaledNubSize, unscaledNubSize}, IM_COL32(200,200,200,255));
        }

        char valBuf[32];
        snprintf(valBuf, 32, Format.c_str(), *Value);
        ImVec2 valSize = SpriteBatch::MeasureString(valBuf);
        ImVec2 valPos = ImVec2(localX + (unscaledWidth - valSize.x) / 2.0f, localY + (unscaledHeight - valSize.y) / 2.0f);
        
        std::string shadowText = "<c=#000000>" + std::string(valBuf) + "</c>";
        std::string whiteText = "<c=#FFFFFF>" + std::string(valBuf) + "</c>";
        
        SpriteBatch::DrawStringOnCtrl(this, shadowText, nullptr, ImVec2(valPos.x + 1.0f/scale, valPos.y + 1.0f/scale));
        SpriteBatch::DrawStringOnCtrl(this, whiteText, nullptr, valPos);
    }

}
}
