#include "../../nexus-sdk-resource.h"
#include "Slider.h"
#include <NexusSDK.h>


#include <nexus-imgui/imgui.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace NexusSDK {
namespace UI {



    void Slider::OnDraw(const Rectangle& bounds, float scale) {
        if (!Value) return;

        float height = 24.0f * scale;
        ImVec2 pos = bounds.GetMin();
        float width = bounds.Width > 0.0f ? bounds.Width : 200.0f * scale;

        if (TextLabel) {
            float spacing = 8.0f * scale;
            ImVec2 textSize = TextLabel->CalcSize();
            float offset = (height - textSize.y) / 2.0f;
            
            Rectangle textBounds;
            textBounds.X = pos.x;
            textBounds.Y = pos.y + offset;
            textBounds.Width = LabelWidth > 0.0f ? LabelWidth * scale : textSize.x;
            textBounds.Height = textSize.y;
            
            TextLabel->Draw(textBounds, scale);
            
            float labelSpace = LabelWidth > 0.0f ? LabelWidth * scale : textSize.x + spacing;
            pos.x += labelSpace;
            width -= labelSpace;
        }

        ImGui::SetCursorScreenPos(pos);

        float nubSize = 20.0f * scale;
        float capW = 4.0f * scale;
        float boundW = 3.0f * scale;
        float usableWidth = width - (2.0f * boundW) - nubSize;

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

        // Draw track
        Texture_t* texTrack = NexusSDK::Content->GetTexture(IDB_TRACKBAR_ATLAS_BG);
        if (texTrack) {
            float trackHeight = 16.0f * scale;
            
            ImVec2 tPos = ImVec2(pos.x, pos.y + (height - trackHeight) / 2.0f);
            
            // Left cap
            ImGui::GetWindowDrawList()->AddImage((ImTextureID)texTrack->Resource, 
                tPos, ImVec2(tPos.x + capW, tPos.y + trackHeight),
                ImVec2(0, 0), ImVec2(4.0f / 256.0f, 1), ImGui::GetColorU32(IM_COL32_WHITE));
                
            // Middle
            ImGui::GetWindowDrawList()->AddImage((ImTextureID)texTrack->Resource, 
                ImVec2(tPos.x + capW, tPos.y), ImVec2(tPos.x + width - capW, tPos.y + trackHeight),
                ImVec2(4.0f / 256.0f, 0), ImVec2((256.0f - 4.0f) / 256.0f, 1), ImGui::GetColorU32(IM_COL32_WHITE));
                
            // Right cap
            ImGui::GetWindowDrawList()->AddImage((ImTextureID)texTrack->Resource, 
                ImVec2(tPos.x + width - capW, tPos.y), ImVec2(tPos.x + width, tPos.y + trackHeight),
                ImVec2((256.0f - 4.0f) / 256.0f, 0), ImVec2(1, 1), ImGui::GetColorU32(IM_COL32_WHITE));
        } else {
            ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), ImGui::GetColorU32(IM_COL32(50,50,50,255)));
        }

        // Draw nub
        Texture_t* texNub = NexusSDK::Content->GetTexture(IDB_TRACKBAR_NUB);
        float percent = std::clamp((*Value - Min) / (Max - Min), 0.0f, 1.0f);
        float nubCenter = pos.x + boundW + (nubSize / 2.0f) + (usableWidth * percent);
        ImVec2 nubMin = ImVec2(nubCenter - nubSize / 2.0f, pos.y + (height - nubSize) / 2.0f);
        ImVec2 nubMax = ImVec2(nubMin.x + nubSize, nubMin.y + nubSize);

        if (texNub) {
            ImGui::GetWindowDrawList()->AddImage((ImTextureID)texNub->Resource, nubMin, nubMax, ImVec2(0,0), ImVec2(1,1), ImGui::GetColorU32(IM_COL32_WHITE));
        } else {
            ImGui::GetWindowDrawList()->AddRectFilled(nubMin, nubMax, ImGui::GetColorU32(IM_COL32(200,200,200,255)));
        }

        char valBuf[32];
        snprintf(valBuf, 32, Format.c_str(), *Value);
        ImVec2 valSize = ImGui::CalcTextSize(valBuf);
        ImVec2 valPos = ImVec2(pos.x + (width - valSize.x) / 2.0f, pos.y + (height - valSize.y) / 2.0f);
        // Draw with drop shadow for readability
        ImGui::GetWindowDrawList()->AddText(ImVec2(valPos.x + 1, valPos.y + 1), ImGui::GetColorU32(IM_COL32(0,0,0,255)), valBuf);
        ImGui::GetWindowDrawList()->AddText(valPos, ImGui::GetColorU32(IM_COL32(255,255,255,255)), valBuf);
    }

}
}
