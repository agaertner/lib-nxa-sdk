#pragma once

#include "ControlBase.h"
#include "Label.h"
#include <string>
#include <memory>
#include <functional>

namespace NexusSDK {
namespace UI {

class Button : public ControlBase {
public:
    Button(const std::string& id, const std::string& text = "");
    virtual ~Button() = default;

    std::shared_ptr<Label> TextLabel;
    std::function<void()> OnClick;
    
    bool ForceHover = false;
    void ForceClick();

protected:
    virtual void OnDraw(const Rectangle& bounds) override;



    virtual void DoMouseLeftDown(const MouseEventArgs& args) override;
    virtual void DoMouseLeftUp(const MouseEventArgs& args) override;
    virtual void DoMouseLeft(const MouseEventArgs& args) override;

private:
    bool m_isPressed = false;
    float m_animState = 0.0f;
    const int ATLAS_SPRITE_WIDTH = 350;
    const int ATLAS_SPRITE_HEIGHT = 20;
    const int ANIM_FRAME_COUNT = 8;
    const float ANIM_FRAME_TIME = 0.25f;
};

} // namespace UI
} // namespace NexusSDK
