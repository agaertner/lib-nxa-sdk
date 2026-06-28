#pragma once

#include "ControlBase.h"
#include "../SpriteBatch.h"
#include <string>
#include <map>
#include <functional>

namespace NexusSDK {
namespace UI {

class Label : public ControlBase {
public:
    Label();
    Label(const std::string& markupText);
    virtual ~Label() = default;

    // Markup Parsing
    void SetMarkupText(const std::string& markupText);
    void SetText(const MarkupString& markupText);

    // Layout
    ImVec2 CalcSize() const;

    // Link Management
    void RegisterAction(const std::string& actionID, std::function<void()> callback);

    // Settings
    bool WrapText = true;

protected:
    virtual void OnDraw(const Rectangle& bounds) override;

    virtual ImVec2 GetAutoSize() const override {
        float scale = UIScale::Get();
        ImVec2 size = CalcSize();
        if (size.y == 0.0f) size.y = 16.0f * scale;
        return ImVec2(size.x / scale, size.y / scale);
    }

private:
    MarkupString m_text;
    std::map<std::string, std::function<void()>> m_actionCallbacks;
};

} // namespace UI
} // namespace NexusSDK
