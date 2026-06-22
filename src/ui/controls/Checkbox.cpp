#include "Checkbox.h"
#include "../../../NexusSDK.h"
#include "../../../nexus-sdk-resource.h"
#include "../UIContext.h"
#include <imgui/imgui.h>

namespace NexusSDK {
namespace UI {

    std::shared_ptr<AsyncTexture> Checkbox::s_texUnchecked;
    std::shared_ptr<AsyncTexture> Checkbox::s_texUncheckedActive;
    std::shared_ptr<AsyncTexture> Checkbox::s_texUncheckedDisabled;
    std::shared_ptr<AsyncTexture> Checkbox::s_texChecked;
    std::shared_ptr<AsyncTexture> Checkbox::s_texCheckedActive;
    std::shared_ptr<AsyncTexture> Checkbox::s_texCheckedDisabled;

    void Checkbox::InitializeTextures() {
        if (!s_texUnchecked && ContextAPI) {
            s_texUnchecked = std::make_shared<AsyncTexture>("cb-unchecked", IDB_CHECKBOX_UNCHECKED, ContextAPI, ContextModule);
            s_texUnchecked->Load();
            s_texUncheckedActive = std::make_shared<AsyncTexture>("cb-unchecked-active", IDB_CHECKBOX_UNCHECKED_ACTIVE, ContextAPI, ContextModule);
            s_texUncheckedActive->Load();
            s_texUncheckedDisabled = std::make_shared<AsyncTexture>("cb-unchecked-disabled", IDB_CHECKBOX_UNCHECKED_DISABLED, ContextAPI, ContextModule);
            s_texUncheckedDisabled->Load();
            s_texChecked = std::make_shared<AsyncTexture>("cb-checked", IDB_CHECKBOX_CHECKED, ContextAPI, ContextModule);
            s_texChecked->Load();
            s_texCheckedActive = std::make_shared<AsyncTexture>("cb-checked-active", IDB_CHECKBOX_CHECKED_ACTIVE, ContextAPI, ContextModule);
            s_texCheckedActive->Load();
            s_texCheckedDisabled = std::make_shared<AsyncTexture>("cb-checked-disabled", IDB_CHECKBOX_CHECKED_DISABLED, ContextAPI, ContextModule);
            s_texCheckedDisabled->Load();
        }
    }

    void Checkbox::OnRender() {
        if (!Value) return;

        InitializeTextures();

        ImVec2 size = ImVec2(36, 36); 

        if (TextLabel) {
            float currentY = ImGui::GetCursorPosY();
            float offset = (size.y - ImGui::GetFontSize()) / 2.0f;
            ImGui::SetCursorPosY(currentY + offset);
            
            TextLabel->Render();
            
            ImGui::SameLine();
            ImGui::SetCursorPosY(currentY);
        }

        ImVec2 pos = ImGui::GetCursorScreenPos();

        if (ImGui::InvisibleButton(m_id.c_str(), size)) {
            *Value = !(*Value);
            if (OnCheckedChanged) {
                OnCheckedChanged(*Value);
            }
            if (ContextAudio) {
                ContextAudio->Play(IDR_AUDIO_CLICK);
            }
        }

        bool isHovered = ImGui::IsItemHovered();
        bool isChecked = *Value;
        
        Texture_t* tex = nullptr;
        if (isChecked) {
            tex = isHovered ? s_texCheckedActive->Get() : s_texChecked->Get();
        } else {
            tex = isHovered ? s_texUncheckedActive->Get() : s_texUnchecked->Get();
        }

        if (tex) {
            ImGui::GetWindowDrawList()->AddImage((ImTextureID)tex->Resource, pos, ImVec2(pos.x + size.x, pos.y + size.y));
        } else {
            ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), isChecked ? IM_COL32(0,255,0,255) : IM_COL32(255,0,0,255));
        }
    }

}
}
