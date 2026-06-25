#include "../../nexus-sdk-resource.h"
#include "Checkbox.h"
#include <NexusSDK.h>


#include <imgui.h>

namespace NexusSDK {
namespace UI {



    void Checkbox::OnDraw(const Rectangle& bounds, float scale) {
        if (!Value) return;

        ImVec2 interactSize = ImVec2(16.0f * scale, 16.0f * scale); 
        ImVec2 drawSize = ImVec2(32.0f * scale, 32.0f * scale); 

        ImVec2 pos = bounds.GetMin();
        ImGui::SetCursorScreenPos(pos);

        if (ImGui::InvisibleButton(m_id.c_str(), interactSize)) {
            *Value = !(*Value);
            if (OnCheckedChanged) {
                OnCheckedChanged(*Value);
            }
            NexusSDK::Audio->Play(IDR_AUDIO_BUTTON_CLICK);
        }

        bool isHovered = ImGui::IsItemHovered();
        bool isChecked = *Value;
        
        Texture_t* tex = nullptr;
        if (*Value) {
            tex = NexusSDK::Content->GetTexture(isHovered ? IDB_CHECKBOX_CHECKED_ACTIVE : IDB_CHECKBOX_CHECKED);
        } else {
            tex = NexusSDK::Content->GetTexture(isHovered ? IDB_CHECKBOX_UNCHECKED_ACTIVE : IDB_CHECKBOX_UNCHECKED);
        }

        ImVec2 drawPos = ImVec2(pos.x - (drawSize.x - interactSize.x) / 2.0f, pos.y - (drawSize.y - interactSize.y) / 2.0f);

        if (tex) {
            ImGui::GetWindowDrawList()->AddImage((ImTextureID)tex->Resource, drawPos, ImVec2(drawPos.x + drawSize.x, drawPos.y + drawSize.y), ImVec2(0,0), ImVec2(1,1), ImGui::GetColorU32(IM_COL32_WHITE));
        } else {
            ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + interactSize.x, pos.y + interactSize.y), isChecked ? ImGui::GetColorU32(IM_COL32(0,255,0,255)) : ImGui::GetColorU32(IM_COL32(255,0,0,255)));
        }

        if (TextLabel) {
            float spacing = 4.0f * scale;
            ImVec2 textSize = TextLabel->CalcSize();
            float offset = (interactSize.y - textSize.y) / 2.0f;

            Rectangle textBounds;
            textBounds.X = pos.x + interactSize.x + spacing;
            textBounds.Y = pos.y + offset;
            textBounds.Width = textSize.x;
            textBounds.Height = textSize.y;

            TextLabel->Draw(textBounds, scale);
        }
    }

}
}
