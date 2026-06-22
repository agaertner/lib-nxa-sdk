#include "Checkbox.h"
#include "../../../NexusSDK.h"
#include "../../../nexus-sdk-resource.h"

#include <imgui/imgui.h>

namespace NexusSDK {
namespace UI {



    void Checkbox::OnRender() {
        if (!Value) return;



        ImVec2 interactSize = ImVec2(16, 16); 
        ImVec2 drawSize = ImVec2(32, 32); 

        ImVec2 pos = ImGui::GetCursorScreenPos();

        if (ImGui::InvisibleButton(m_id.c_str(), interactSize)) {
            *Value = !(*Value);
            if (OnCheckedChanged) {
                OnCheckedChanged(*Value);
            }
            NexusSDK::Audio->Play(IDR_AUDIO_CLICK);
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
            ImGui::GetWindowDrawList()->AddImage((ImTextureID)tex->Resource, drawPos, ImVec2(drawPos.x + drawSize.x, drawPos.y + drawSize.y));
        } else {
            ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + interactSize.x, pos.y + interactSize.y), isChecked ? IM_COL32(0,255,0,255) : IM_COL32(255,0,0,255));
        }

        if (TextLabel) {
            ImGui::SameLine(0.0f, 4.0f); // Even closer spacing
            float currentY = ImGui::GetCursorPosY();
            float offset = (interactSize.y - ImGui::GetFontSize()) / 2.0f;
            ImGui::SetCursorPosY(currentY + offset);
            TextLabel->Render();
        }
    }

}
}
