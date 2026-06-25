#include "../../nexus-sdk-resource.h"
#include "Button.h"
#include <NexusSDK.h>


#include <imgui.h>
#include <imgui_internal.h>

namespace NexusSDK {
namespace UI {

    Button::Button(const std::string& id, const std::string& text)
        : ControlBase()
    {
        m_id = id;
        TextLabel = std::make_shared<Label>("<c=#000000>" + text + "</c>");
        TextLabel->WrapText = false; // Buttons usually don't wrap
        
        // Button handles input natively via InvisibleButton now, ignore ControlBase events
        InputCapture = CaptureType::None;
    }

    void Button::OnDraw(const Rectangle& bounds, float scale) {
        ImVec2 pos = bounds.GetMin();
        ImVec2 size = ImVec2(bounds.Width, bounds.Height);

        // Natively intercept input using InvisibleButton to completely bypass ControlBase/Modal anomalies
        ImGui::SetCursorScreenPos(pos);
        bool isClicked = ImGui::InvisibleButton(("##inv_" + m_id).c_str(), size);
        bool isHovered = ImGui::IsItemHovered();
        bool isActive = ImGui::IsItemActive();

        m_isHovered = isHovered;
        if (isClicked) {
            ForceClick(); // Reuses our existing force click method
        }

        float dt = ImGui::GetIO().DeltaTime;
        if (m_isHovered || ForceHover) {
            m_animState += dt * (static_cast<float>(ANIM_FRAME_COUNT) / ANIM_FRAME_TIME);
        } else {
            m_animState -= dt * (static_cast<float>(ANIM_FRAME_COUNT) / ANIM_FRAME_TIME);
        }

        if (m_animState > static_cast<float>(ANIM_FRAME_COUNT)) m_animState = static_cast<float>(ANIM_FRAME_COUNT);
        if (m_animState < 0.0f) m_animState = 0.0f;

        int frame = static_cast<int>(m_animState);
        if (frame >= ANIM_FRAME_COUNT) frame = ANIM_FRAME_COUNT - 1;

        if (isActive || m_isPressed) {
            frame = ANIM_FRAME_COUNT - 1; // Last frame is fully pressed
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        Texture_t* texStates = NexusSDK::Content->GetTexture(IDB_BUTTON_STATES);
        Texture_t* texBorder = NexusSDK::Content->GetTexture(IDB_BUTTON_BORDER);

        if (texStates) {
            float totalWidth = static_cast<float>(texStates->Width);
            float totalHeight = static_cast<float>(texStates->Height);
            
            float uv0x = (frame * ATLAS_SPRITE_WIDTH) / totalWidth;
            float uv0y = 0.0f;
            float uv1x = ((frame + 1) * ATLAS_SPRITE_WIDTH) / totalWidth;
            float uv1y = ATLAS_SPRITE_HEIGHT / totalHeight;

            drawList->AddImage((ImTextureID)texStates->Resource, 
                               ImVec2(pos.x + 3, pos.y + 3), 
                               ImVec2(pos.x + size.x - 3, pos.y + size.y - 2), 
                               ImVec2(uv0x, uv0y), 
                               ImVec2(uv1x, uv1y), ImGui::GetColorU32(IM_COL32_WHITE));
        }

        if (texBorder) {
            float bw = 4.0f;
            float bh = 4.0f;

            // Top
            drawList->AddImage((ImTextureID)texBorder->Resource,
                               ImVec2(pos.x + 2, pos.y),
                               ImVec2(pos.x + size.x - 3, pos.y + 4),
                               ImVec2(0/bw, 0/bh), ImVec2(1/bw, 4/bh), ImGui::GetColorU32(IM_COL32_WHITE));
            // Right
            drawList->AddImage((ImTextureID)texBorder->Resource,
                               ImVec2(pos.x + size.x - 4, pos.y + 2),
                               ImVec2(pos.x + size.x, pos.y + size.y - 1),
                               ImVec2(0/bw, 1/bh), ImVec2(4/bw, 2/bh), ImGui::GetColorU32(IM_COL32_WHITE));
            // Bottom
            drawList->AddImage((ImTextureID)texBorder->Resource,
                               ImVec2(pos.x + 3, pos.y + size.y - 4),
                               ImVec2(pos.x + size.x - 3, pos.y + size.y),
                               ImVec2(1/bw, 0/bh), ImVec2(2/bw, 4/bh), ImGui::GetColorU32(IM_COL32_WHITE));
            // Left
            drawList->AddImage((ImTextureID)texBorder->Resource,
                               ImVec2(pos.x, pos.y + 2),
                               ImVec2(pos.x + 4, pos.y + size.y - 1),
                               ImVec2(0/bw, 3/bh), ImVec2(4/bw, 4/bh), ImGui::GetColorU32(IM_COL32_WHITE));
        }

        if (TextLabel) {
            ImVec2 textSize = TextLabel->CalcSize();
            float textX = pos.x + (size.x - textSize.x) / 2.0f;
            float textY = pos.y + (size.y - textSize.y) / 2.0f;

            Rectangle textBounds;
            textBounds.X = textX;
            textBounds.Y = textY;
            textBounds.Width = textSize.x;
            textBounds.Height = textSize.y;

            TextLabel->Draw(textBounds, scale);
        }
    }

    void Button::DoMouseLeftDown(const MouseEventArgs& args) {
        ControlBase::DoMouseLeftDown(args);
        m_isPressed = true;
    }

    void Button::DoMouseLeftUp(const MouseEventArgs& args) {
        ControlBase::DoMouseLeftUp(args);
        m_isPressed = false;
    }

    void Button::DoMouseLeft(const MouseEventArgs& args) {
        ControlBase::DoMouseLeft(args);
        m_isPressed = false;
    }

    void Button::ForceClick() {
        if (OnClick) OnClick();
        NexusSDK::Audio->Play(IDR_AUDIO_BUTTON_CLICK);
    }

} // namespace UI
} // namespace NexusSDK
