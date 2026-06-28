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
        TextLabel->HorizontalAlignment = Alignment::Center;
        TextLabel->VerticalAlignment = Alignment::Center;
        
        SetSize(128.0f, 26.0f);
        
        // Button handles input natively via InvisibleButton now, ignore ControlBase events
        InputCapture = CaptureType::None;
    }

    void Button::OnDraw(const Rectangle& bounds) {
        float scale = UIScale::Get();
        // Natively intercept input using InvisibleButton to completely bypass ControlBase/Modal anomalies
        bool isClicked = ImGui::InvisibleButton(("##inv_" + m_id).c_str(), ImVec2(bounds.Width, bounds.Height));
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

        AsyncTexture* texStates = NexusSDK::Content->GetTexture(IDB_BUTTON_STATES);
        AsyncTexture* texBorder = NexusSDK::Content->GetTexture(IDB_BUTTON_BORDER);

        if (texStates) {
            float totalWidth = static_cast<float>(texStates->GetWidth());
            float totalHeight = static_cast<float>(texStates->GetHeight());
            
            if (totalWidth > 0.0f && totalHeight > 0.0f) {
                float uv0x = (frame * ATLAS_SPRITE_WIDTH) / totalWidth;
                float uv0y = 0.0f;
                float uv1x = ((frame + 1) * ATLAS_SPRITE_WIDTH) / totalWidth;
                float uv1y = ATLAS_SPRITE_HEIGHT / totalHeight;

                SpriteBatch::DrawTexture(texStates, 
                                   ImVec2(bounds.X + 3, bounds.Y + 3), 
                                   ImVec2(bounds.X + bounds.Width - 3, bounds.Y + bounds.Height - 2), 
                                   ImVec2(uv0x, uv0y), 
                                   ImVec2(uv1x, uv1y));
            }
        }

        if (texBorder) {
            float bw = 4.0f;
            float bh = 4.0f;

            // Top
            SpriteBatch::DrawTexture(texBorder,
                               ImVec2(bounds.X + 2, bounds.Y),
                               ImVec2(bounds.X + bounds.Width - 3, bounds.Y + 4),
                               ImVec2(0/bw, 0/bh), ImVec2(1/bw, 4/bh));
            // Right
            SpriteBatch::DrawTexture(texBorder,
                               ImVec2(bounds.X + bounds.Width - 4, bounds.Y + 2),
                               ImVec2(bounds.X + bounds.Width, bounds.Y + bounds.Height - 1),
                               ImVec2(0/bw, 1/bh), ImVec2(4/bw, 2/bh));
            // Bottom
            SpriteBatch::DrawTexture(texBorder,
                               ImVec2(bounds.X + 3, bounds.Y + bounds.Height - 4),
                               ImVec2(bounds.X + bounds.Width - 3, bounds.Y + bounds.Height),
                               ImVec2(1/bw, 0/bh), ImVec2(2/bw, 4/bh));
            // Left
            SpriteBatch::DrawTexture(texBorder,
                               ImVec2(bounds.X, bounds.Y + 2),
                               ImVec2(bounds.X + 4, bounds.Y + bounds.Height - 1),
                               ImVec2(0/bw, 3/bh), ImVec2(4/bw, 4/bh));
        }

        if (TextLabel) {
            TextLabel->Draw(bounds);
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
