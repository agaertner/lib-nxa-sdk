#pragma once

#include "ControlBase.h"
#include "Button.h"
#include "Label.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace NexusSDK {
namespace UI {

enum class DialogIcon {
    None,
    Exclamation,
    Question,
    Present
};

struct DialogButton {
    std::string Text;
    bool Selected = false;
    std::function<void()> OnClick = nullptr;
    
    static DialogButton OK();
    static DialogButton Confirm();
    static DialogButton Accept();
    static DialogButton Cancel();
    static DialogButton Yes();
    static DialogButton No();
    static DialogButton Ignore();
    static DialogButton Close();
};

class StandardDialog : public ControlBase {
public:
    StandardDialog(const std::string& text, DialogIcon sysIcon, const std::vector<DialogButton>& buttons);
    ~StandardDialog() = default;

    static void Show(const std::string& text, DialogIcon sysIcon = DialogIcon::None, std::vector<DialogButton> buttons = { DialogButton::OK() });
    
    // Must be called in the addon's render loop
    static void RenderAll();

protected:
    virtual void OnRender() override;

private:
    void CalculateLayout();
    void Close();

    std::string m_text;
    DialogIcon m_icon;
    std::vector<DialogButton> m_buttonsData;
    
    // UI elements
    std::shared_ptr<Label> m_label;
    
    // Layout
    bool m_layoutCalculated = false;
    float m_maxIconSize = 64.0f;
    float m_buttonWidth = 117.0f;
    float m_buttonHeight = 24.0f;

    static std::vector<std::shared_ptr<StandardDialog>> s_activeDialogs;
};

} // namespace UI
} // namespace NexusSDK
