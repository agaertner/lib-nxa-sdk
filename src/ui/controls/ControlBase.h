#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <imgui/imgui.h>
#include "../../utils/ImID.h"

namespace NexusSDK {
namespace UI {

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
    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    void SetPosition(ImVec2 pos) { m_position = pos; m_hasAbsolutePosition = true; }
    ImVec2 GetPosition() const { return m_position; }
    void ClearPosition() { m_hasAbsolutePosition = false; }

    void SetSize(ImVec2 size) { m_size = size; }
    ImVec2 GetSize() const { return m_size; }

    void AddChild(std::shared_ptr<ControlBase> child) {
        child->m_parent = weak_from_this();
        m_children.push_back(child);
    }

    void ClearChildren() {
        m_children.clear();
    }

    std::shared_ptr<ControlBase> GetParent() const {
        return m_parent.lock();
    }

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

    // The main render pass
    void Render() {
        if (!m_visible) return;

        ImGui::PushID(m_id.c_str());

        if (m_hasAbsolutePosition) {
            ImGui::SetCursorPos(m_position);
        }

        // Store screen position before derived classes advance the ImGui cursor
        ImVec2 screenPos = ImGui::GetCursorScreenPos();

        // Call the derived class's actual drawing logic
        OnRender();

        HandleMouseEvents(screenPos);

        ImGui::PopID();
    }

protected:
    // Derived classes can override this to implement custom rendering logic.
    // Containers should call RenderChildren() inside their overridden OnRender().
    virtual void OnRender() {
        RenderChildren();
    }

    void RenderChildren() {
        for (auto& child : m_children) {
            child->Render();
        }
    }

    std::string m_id;
    bool m_visible = true;
    ImVec2 m_position = {0, 0};
    ImVec2 m_size = {0, 0};
    bool m_hasAbsolutePosition = false;

    bool m_isHovered = false;
    float m_hoverTime = 0.0f;

    std::vector<std::shared_ptr<ControlBase>> m_children;
    std::weak_ptr<ControlBase> m_parent;

private:
    void HandleMouseEvents(ImVec2 minScreenPos) {
        // Event bounds check relies on size.
        if (m_size.x > 0 && m_size.y > 0) {
            ImVec2 maxScreenPos = ImVec2(minScreenPos.x + m_size.x, minScreenPos.y + m_size.y);
            
            // IsMouseHoveringRect handles clipping automatically
            bool isHoveredNow = ImGui::IsMouseHoveringRect(minScreenPos, maxScreenPos, true);
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
                    BasicTooltip->Render();
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
