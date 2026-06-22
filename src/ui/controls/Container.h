#pragma once

#include "ControlBase.h"
#include <imgui/imgui.h>

namespace NexusSDK {
namespace UI {

class Container : public ControlBase {
public:
    Container() : ControlBase() {}
    virtual ~Container() = default;

    // Optional padding/margin logic could go here in the future
    
protected:
    virtual void OnRender() override {
        // ImGui::BeginChild creates an isolated, scrollable region with its own clipping rect.
        // The ID is pushed by the parent Control::Render(), but BeginChild needs a string ID as well to track scrolling state.
        
        // If m_size is 0,0, ImGui::BeginChild automatically uses the remaining available space.
        bool isVisible = ImGui::BeginChild(m_id.c_str(), m_size, false, ImGuiWindowFlags_NoBackground);
        
        if (isVisible) {
            RenderChildren();
        }
        
        ImGui::EndChild();
    }
};

} // namespace UI
} // namespace NexusSDK
