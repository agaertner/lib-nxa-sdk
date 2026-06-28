#include "../../nexus-sdk-resource.h"
#include "Checkbox.h"
#include <NexusSDK.h>
#include "../SpriteBatch.h"

#include <imgui.h>

namespace NexusSDK {
namespace UI {



    void Checkbox::OnDraw(const Rectangle& bounds) {
        if (!Value) return;
        float scale = UIScale::Get();

        ImVec2 interactSize = ImVec2(16.0f * scale, 16.0f * scale); 

        if (ImGui::InvisibleButton(m_id.c_str(), interactSize)) {
            *Value = !(*Value);
            if (OnCheckedChanged) {
                OnCheckedChanged(*Value);
            }
            NexusSDK::Audio->Play(IDR_AUDIO_BUTTON_CLICK);
        }

        bool isHovered = ImGui::IsItemHovered();
        bool isChecked = *Value;
        
        AsyncTexture* texAsync = nullptr;
        if (*Value) {
            texAsync = NexusSDK::Content->GetTexture(isHovered ? IDB_CHECKBOX_CHECKED_ACTIVE : IDB_CHECKBOX_CHECKED);
        } else {
            texAsync = NexusSDK::Content->GetTexture(isHovered ? IDB_CHECKBOX_UNCHECKED_ACTIVE : IDB_CHECKBOX_UNCHECKED);
        }
        float unscaledInteractW = 16.0f;
        float unscaledInteractH = 16.0f;
        if (texAsync && texAsync->GetWidth() > 0) {
            float unscaledDrawW = 32.0f;
            float unscaledDrawH = 32.0f;

            Rectangle destRect;
            destRect.X = -(unscaledDrawW - unscaledInteractW) / 2.0f;
            destRect.Y = -(unscaledDrawH - unscaledInteractH) / 2.0f;
            destRect.Width = unscaledDrawW;
            destRect.Height = unscaledDrawH;
            SpriteBatch::DrawTextureOnCtrl(this, texAsync, destRect);
        } else {
            Rectangle fillRect = { 0, 0, unscaledInteractW, unscaledInteractH };
            SpriteBatch::DrawFilledRectOnCtrl(this, fillRect, isChecked ? IM_COL32(0,255,0,255) : IM_COL32(255,0,0,255));
        }

        if (TextLabel) {
            float unscaledSpacing = 4.0f;
            ImVec2 textSize = TextLabel->CalcSize();
            float unscaledOffset = (unscaledInteractH - textSize.y) / 2.0f;

            Rectangle textBounds;
            textBounds.X = bounds.X + interactSize.x + (unscaledSpacing * scale);
            textBounds.Y = bounds.Y + (unscaledOffset * scale);
            textBounds.Width = textSize.x * scale;
            textBounds.Height = textSize.y * scale;

            TextLabel->Draw(textBounds);
        }
    }

}
}
