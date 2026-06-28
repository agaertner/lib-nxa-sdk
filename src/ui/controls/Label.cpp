#include "Label.h"

namespace NexusSDK {
namespace UI {

Label::Label() : ControlBase() {}

Label::Label(const std::string& markupText) : ControlBase() {
    SetMarkupText(markupText);
}

void Label::RegisterAction(const std::string& actionID, std::function<void()> callback) {
    m_actionCallbacks[actionID] = callback;
}

void Label::SetMarkupText(const std::string& markupText) {
    m_text.Parse(markupText);
}

void Label::SetText(const MarkupString& markupText) {
    m_text = markupText;
}

ImVec2 Label::CalcSize() const {
    float wrapWidth = WrapText ? (m_size.x > 0 ? m_size.x : (m_absoluteBounds.Width > 0 ? m_absoluteBounds.Width : 0.0f)) : 0.0f;
    return SpriteBatch::MeasureString(m_text, this->Font, wrapWidth);
}

void Label::OnDraw(const Rectangle& bounds) {
    float wrapWidth = WrapText ? bounds.Width : 0.0f;
    std::string clickedAction = SpriteBatch::DrawStringOnCtrl(this, m_text, this->Font, ImVec2(0, 0), wrapWidth);
    
    if (!clickedAction.empty() && m_actionCallbacks.count(clickedAction)) {
        m_actionCallbacks[clickedAction]();
    }
}

} // namespace UI
} // namespace NexusSDK
