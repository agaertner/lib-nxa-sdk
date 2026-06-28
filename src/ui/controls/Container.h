#pragma once

#include "ControlBase.h"
#include <imgui.h>
#include <vector>
#include <memory>
#include <algorithm>

namespace NexusSDK {
namespace UI {

class Container : public ControlBase {
public:
    Container() : ControlBase() {
        InputCapture = CaptureType::None;
    }
    virtual ~Container() = default;



    void AddChild(std::shared_ptr<ControlBase> child) {
        if (!child) return;
        
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it == m_children.end()) {
            m_children.push_back(child);
        }
        
        if (child->GetParent() != this) {
            child->SetParent(this);
        }
    }

    void RemoveChild(std::shared_ptr<ControlBase> child) {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end()) {
            auto childPtr = *it;
            m_children.erase(it);
            childPtr->SetParent(nullptr);
        }
    }

    void ClearChildren() {
        while (!m_children.empty()) {
            m_children.back()->SetParent(nullptr);
        }
    }

    const std::vector<std::shared_ptr<ControlBase>>& GetChildren() const { return m_children; }

    virtual CaptureType HitTest(ImVec2 mousePos) override {
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

protected:
    virtual ImVec2 GetAutoSize() const override {
        float maxX = 0.0f;
        float maxY = 0.0f;
        for (const auto& child : m_children) {
            if (!child->IsVisible()) continue;
            float childRight = child->Left() + child->GetSize().x;
            float childBottom = child->Top() + child->GetSize().y;
            if (childRight > maxX) maxX = childRight;
            if (childBottom > maxY) maxY = childBottom;
        }
        return ImVec2(
            (std::max)(maxX, m_size.x > 0.0f ? m_size.x : 0.0f),
            (std::max)(maxY, m_size.y > 0.0f ? m_size.y : 0.0f)
        );
    }

    virtual void OnDraw(const Rectangle& bounds) override {
        DrawChildren(bounds);
    }

    void DrawChildren(const Rectangle& parentBounds) {
        float scale = UIScale::Get();
        for (auto& child : m_children) {
            Rectangle childBounds;
            childBounds.X = parentBounds.X + (child->Left() * scale);
            childBounds.Y = parentBounds.Y + (child->Top() * scale);
            childBounds.Width = child->GetSize().x * scale;
            childBounds.Height = child->GetSize().y * scale;
            
            child->Draw(childBounds);
        }
    }

    std::vector<std::shared_ptr<ControlBase>> m_children;
};



} // namespace UI
} // namespace NexusSDK
