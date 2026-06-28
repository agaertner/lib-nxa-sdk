#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <imgui.h>
#include "../../utils/ImID.h"
#include "../../utils/ImStateGuards.h"

#include "../UIScale.h"
#include "../../services/NexusService.h"

#include "../../utils/AsyncFont.h"

namespace NexusSDK {
namespace UI {

class Container;

enum class CaptureType {
    None = 0,
    Capture = 1,
    Filter = 2
};

enum class Alignment {
    Start = 0,
    Center = 1,
    End = 2
};

struct Rectangle {
    float X, Y, Width, Height;

    ImVec2 GetMin() const { return ImVec2(X, Y); }
    ImVec2 GetMax() const { return ImVec2(X + Width, Y + Height); }

    Rectangle ToBounds(const Rectangle& localBounds) const {
        float scale = UIScale::Get();
        return {
            X + (localBounds.X * scale),
            Y + (localBounds.Y * scale),
            localBounds.Width * scale,
            localBounds.Height * scale
        };
    }
};

struct MouseEventArgs {
    ImVec2 Position;
    bool IsPressed;
};

class ControlBase : public std::enable_shared_from_this<ControlBase> {
public:
    ControlBase() {
        m_id = ImID::Create("ControlBase");
    }
    virtual ~ControlBase() = default;

    // Retained State
    float Opacity = 1.0f;
    CaptureType InputCapture = CaptureType::Capture;
    AsyncFont* Font = nullptr;
    Alignment HorizontalAlignment = Alignment::Start;
    Alignment VerticalAlignment = Alignment::Start;
    bool IgnoreLayoutCursor = false;
    
    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    void SetPosition(ImVec2 pos) { m_position = pos; }
    void SetPosition(float x, float y) { SetPosition(ImVec2(x, y)); }
    ImVec2 GetPosition() const { return m_position; }
    
    // Layout Helpers
    float Left() const { return m_position.x; }
    void Left(float x) { m_position.x = x; }
    float Top() const { return m_position.y; }
    void Top(float y) { m_position.y = y; }

    void SetSize(ImVec2 size) { m_size = size; }
    void SetSize(float width, float height) { SetSize(ImVec2(width, height)); }
    
    float Width() const { return m_size.x; }
    void Width(float w) { m_size.x = w; }
    float Height() const { return m_size.y; }
    void Height(float h) { m_size.y = h; }
    
    virtual ImVec2 GetAutoSize() const {
        return ImVec2(0.0f, 0.0f);
    }

    ImVec2 GetSize() const {
        ImVec2 size = m_size;
        ImVec2 autoSize = GetAutoSize();
        if (size.x <= 0.0f) size.x = autoSize.x;
        if (size.y <= 0.0f) size.y = autoSize.y;
        return size;
    }
    
    Rectangle GetBounds() const { return { m_position.x, m_position.y, m_size.x, m_size.y }; }
    Rectangle GetAbsoluteBounds() const { return m_absoluteBounds; }

    Container* GetParent() const {
        return m_parent;
    }

    void SetParent(Container* parent);

    const std::string& GetID() const { return m_id; }

    // Tooltips
    std::shared_ptr<ControlBase> Tooltip;
    float TooltipDelay = 0.5f;

    // Functional Callbacks
    std::function<void(const MouseEventArgs&)> OnMouseLeftDown;
    std::function<void(const MouseEventArgs&)> OnMouseLeftUp;
    std::function<void(const MouseEventArgs&)> OnMouseRightDown;
    std::function<void(const MouseEventArgs&)> OnMouseRightUp;
    std::function<void(const MouseEventArgs&)> OnMouseEntered;
    std::function<void(const MouseEventArgs&)> OnMouseLeft;
    std::function<void(const MouseEventArgs&)> OnMouseHover;
    std::function<void(const MouseEventArgs&)> OnMouseMoved;

    // Virtual Handlers for Subclasses
    virtual void DoMouseLeftDown(const MouseEventArgs& args) { if (OnMouseLeftDown) OnMouseLeftDown(args); }
    virtual void DoMouseLeftUp(const MouseEventArgs& args) { if (OnMouseLeftUp) OnMouseLeftUp(args); }
    virtual void DoMouseRightDown(const MouseEventArgs& args) { if (OnMouseRightDown) OnMouseRightDown(args); }
    virtual void DoMouseRightUp(const MouseEventArgs& args) { if (OnMouseRightUp) OnMouseRightUp(args); }
    virtual void DoMouseEntered(const MouseEventArgs& args) { if (OnMouseEntered) OnMouseEntered(args); }
    virtual void DoMouseLeft(const MouseEventArgs& args) { if (OnMouseLeft) OnMouseLeft(args); }
    virtual void DoMouseHover(const MouseEventArgs& args) { if (OnMouseHover) OnMouseHover(args); }
    virtual void DoMouseMoved(const MouseEventArgs& args) { if (OnMouseMoved) OnMouseMoved(args); }

    virtual CaptureType HitTest(ImVec2 mousePos) {
        if (!m_visible || Opacity <= 0.0f) return CaptureType::None;

        if (m_absoluteBounds.Width > 0 && m_absoluteBounds.Height > 0) {
            if (mousePos.x >= m_absoluteBounds.X && mousePos.x <= (m_absoluteBounds.X + m_absoluteBounds.Width) &&
                mousePos.y >= m_absoluteBounds.Y && mousePos.y <= (m_absoluteBounds.Y + m_absoluteBounds.Height)) {
                return InputCapture;
            }
        }

        return CaptureType::None;
    }

    // The main render pass
    void Draw(const Rectangle& bounds) {
        float scale = UIScale::Get();
        ImVec2 unscaledSize = GetSize();
        float actualW = unscaledSize.x * scale;
        float actualH = unscaledSize.y * scale;

        float finalX = bounds.X;
        float finalY = bounds.Y;
        float finalW = bounds.Width;
        float finalH = bounds.Height;

        if (actualW > 0.0f) {
            if (HorizontalAlignment == Alignment::Center) {
                finalW = actualW;
                finalX = bounds.X + (bounds.Width - finalW) / 2.0f;
            } else if (HorizontalAlignment == Alignment::End) {
                finalW = actualW;
                finalX = bounds.X + bounds.Width - finalW;
            }
        }

        if (actualH > 0.0f) {
            if (VerticalAlignment == Alignment::Center) {
                finalH = actualH;
                finalY = bounds.Y + (bounds.Height - finalH) / 2.0f;
            } else if (VerticalAlignment == Alignment::End) {
                finalH = actualH;
                finalY = bounds.Y + bounds.Height - finalH;
            }
        }

        m_absoluteBounds = { finalX, finalY, finalW, finalH };
        
        if (!m_visible || Opacity <= 0.0f) return;

        IDGuard id(m_id.c_str());
        
        ImFont* fontToUse = this->Font ? this->Font->Get() : nullptr;
        if (!fontToUse && Services::NexusService::Get()) {
            fontToUse = Services::NexusService::Get()->FontUI();
        }
        FontGuard font(fontToUse);
        
        StyleGuard alpha(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * Opacity);

        // Position the ImGui cursor at this control's screen-space origin (if we belong to the layout engine)
        if (!IgnoreLayoutCursor) {
            ImGui::SetCursorScreenPos(m_absoluteBounds.GetMin());
        }

        // Call the derived class's actual drawing logic
        OnDraw(m_absoluteBounds);

        HandleMouseEvents(m_absoluteBounds);
    }

protected:
    // Derived classes can override this to implement custom rendering logic.
    virtual void OnDraw(const Rectangle& bounds) {
        // Base implementation does nothing.
    }

    std::string m_id;
    bool m_visible = true;
    ImVec2 m_position = {0, 0};
    ImVec2 m_size = {0, 0};
    Rectangle m_absoluteBounds = {0, 0, 0, 0};

    bool m_isHovered = false;
    float m_hoverTime = 0.0f;

    Container* m_parent = nullptr;

private:
    void HandleMouseEvents(const Rectangle& bounds) {
        // Event bounds check relies on size.
        if (bounds.Width > 0 && bounds.Height > 0) {
            ImVec2 minScreenPos = bounds.GetMin();
            ImVec2 maxScreenPos = bounds.GetMax();
            
            // IsMouseHoveringRect handles overlap natively. We disable clipping to bypass modal clip anomalies.
            bool isHoveredNow = ImGui::IsMouseHoveringRect(minScreenPos, maxScreenPos, false);
            ImVec2 mousePos = ImGui::GetMousePos();
            MouseEventArgs args{ mousePos, false };

            // Transition: Enter
            if (isHoveredNow && !m_isHovered) {
                DoMouseEntered(args);
                m_hoverTime = 0.0f;
            } 
            // Transition: Leave
            else if (!isHoveredNow && m_isHovered) {
                DoMouseLeft(args);
                m_hoverTime = 0.0f;
            }

            // Continuous: Hover
            if (isHoveredNow) {
                DoMouseHover(args);

                // Tooltip Rendering
                m_hoverTime += ImGui::GetIO().DeltaTime;
                if (m_hoverTime >= TooltipDelay && Tooltip) {
                    float scale = UIScale::Get();
                    Rectangle tooltipBounds;
                    tooltipBounds.X = mousePos.x + (15.0f * scale);
                    tooltipBounds.Y = mousePos.y + (15.0f * scale);
                    tooltipBounds.Width = Tooltip->GetSize().x * scale;
                    tooltipBounds.Height = Tooltip->GetSize().y * scale;
                    Tooltip->Draw(tooltipBounds);
                }

                // Continuous: Moved
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                if (delta.x != 0.0f || delta.y != 0.0f) {
                    DoMouseMoved(args);
                }

                // Discrete: Clicks
                if (ImGui::IsMouseClicked(0)) { args.IsPressed = true; DoMouseLeftDown(args); }
                if (ImGui::IsMouseReleased(0)) { args.IsPressed = false; DoMouseLeftUp(args); }
                if (ImGui::IsMouseClicked(1)) { args.IsPressed = true; DoMouseRightDown(args); }
                if (ImGui::IsMouseReleased(1)) { args.IsPressed = false; DoMouseRightUp(args); }
            }

            m_isHovered = isHoveredNow;
        }
    }
};

} // namespace UI
} // namespace NexusSDK
