#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <imgui/imgui.h>
#include "../../utils/ImID.h"

namespace NexusSDK {
namespace UI {

enum class CaptureType {
    None = 0,
    Capture = 1,
    Filter = 2
};

struct Rectangle {
    float X, Y, Width, Height;

    ImVec2 GetMin() const { return ImVec2(X, Y); }
    ImVec2 GetMax() const { return ImVec2(X + Width, Y + Height); }
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
    
    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    void SetPosition(ImVec2 pos) { m_position = pos; }
    void SetPosition(float x, float y) { SetPosition(ImVec2(x, y)); }
    ImVec2 GetPosition() const { return m_position; }

    void SetSize(ImVec2 size) { m_size = size; }
    void SetSize(float width, float height) { SetSize(ImVec2(width, height)); }
    
    virtual ImVec2 GetAutoSize(float scale) const {
        return ImVec2(0.0f, 0.0f);
    }

    ImVec2 GetSize(float scale = 1.0f) const {
        ImVec2 size = m_size;
        ImVec2 autoSize = GetAutoSize(scale);
        if (size.x <= 0.0f) size.x = autoSize.x;
        if (size.y <= 0.0f) size.y = autoSize.y;
        return size;
    }
    
    Rectangle GetBounds() const { return { m_position.x, m_position.y, m_size.x, m_size.y }; }

    void AddChild(std::shared_ptr<ControlBase> child) {
        child->m_parent = weak_from_this();
        m_children.push_back(child);
    }

    void ClearChildren() {
        m_children.clear();
    }

    void RemoveChild(std::shared_ptr<ControlBase> child) {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end()) {
            (*it)->m_parent.reset();
            m_children.erase(it);
        }
    }

    std::shared_ptr<ControlBase> GetParent() const {
        return m_parent.lock();
    }

    const std::string& GetID() const { return m_id; }
    const std::vector<std::shared_ptr<ControlBase>>& GetChildren() const { return m_children; }

    // Tooltips
    std::shared_ptr<ControlBase> BasicTooltip;
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

        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
            CaptureType childResult = (*it)->HitTest(mousePos);
            if (childResult != CaptureType::None) {
                return childResult;
            }
        }

        if (m_absoluteBounds.Width > 0 && m_absoluteBounds.Height > 0) {
            if (mousePos.x >= m_absoluteBounds.X && mousePos.x <= (m_absoluteBounds.X + m_absoluteBounds.Width) &&
                mousePos.y >= m_absoluteBounds.Y && mousePos.y <= (m_absoluteBounds.Y + m_absoluteBounds.Height)) {
                return InputCapture;
            }
        }

        return CaptureType::None;
    }

    // The main render pass
    virtual void Draw(const Rectangle& bounds, float scale) {
        m_absoluteBounds = bounds;
        
        if (!m_visible || Opacity <= 0.0f) return;

        ImGui::PushID(m_id.c_str());

        bool pushAlpha = (Opacity < 1.0f);
        if (pushAlpha) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * Opacity);
        }

        // Call the derived class's actual drawing logic
        OnDraw(bounds, scale);

        HandleMouseEvents(bounds, scale);

        if (pushAlpha) {
            ImGui::PopStyleVar();
        }

        ImGui::PopID();
    }

protected:
    // Derived classes can override this to implement custom rendering logic.
    // Containers should call DrawChildren() inside their overridden OnDraw().
    virtual void OnDraw(const Rectangle& bounds, float scale) {
        DrawChildren(bounds, scale);
    }

    void DrawChildren(const Rectangle& parentBounds, float scale) {
        for (auto& child : m_children) {
            Rectangle childBounds;
            childBounds.X = parentBounds.X + (child->GetPosition().x * scale);
            childBounds.Y = parentBounds.Y + (child->GetPosition().y * scale);
            childBounds.Width = child->GetSize(scale).x * scale;
            childBounds.Height = child->GetSize(scale).y * scale;
            
            child->Draw(childBounds, scale);
        }
    }

    std::string m_id;
    bool m_visible = true;
    ImVec2 m_position = {0, 0};
    ImVec2 m_size = {0, 0};
    Rectangle m_absoluteBounds = {0, 0, 0, 0};

    bool m_isHovered = false;
    float m_hoverTime = 0.0f;

    std::vector<std::shared_ptr<ControlBase>> m_children;
    std::weak_ptr<ControlBase> m_parent;

private:
    void HandleMouseEvents(const Rectangle& bounds, float scale) {
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
                if (m_hoverTime >= TooltipDelay && BasicTooltip) {
                    Rectangle tooltipBounds;
                    tooltipBounds.X = mousePos.x + (15.0f * scale);
                    tooltipBounds.Y = mousePos.y + (15.0f * scale);
                    tooltipBounds.Width = BasicTooltip->GetSize(scale).x * scale;
                    tooltipBounds.Height = BasicTooltip->GetSize(scale).y * scale;
                    BasicTooltip->Draw(tooltipBounds, scale);
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
