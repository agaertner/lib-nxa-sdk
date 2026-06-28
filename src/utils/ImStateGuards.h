#pragma once
#include <imgui.h>

namespace NexusSDK {
namespace UI {

    struct FontGuard {
        bool m_pushed;

        FontGuard(ImFont* font) {
            m_pushed = font != nullptr;
            if (m_pushed) {
                ImGui::PushFont(font);
            }
        }

        ~FontGuard() {
            if (m_pushed) {
                ImGui::PopFont();
            }
        }
    };

    struct ColorGuard {
        bool m_pushed;

        ColorGuard(bool condition, float r, float g, float b, float a = 1.0f) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, a));
        }

        ColorGuard(float r, float g, float b, float a = 1.0f) : ColorGuard(true, r, g, b, a) {}

        ColorGuard(bool condition, ImGuiCol idx, float r, float g, float b, float a = 1.0f) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleColor(idx, ImVec4(r, g, b, a));
        }

        ColorGuard(ImGuiCol idx, float r, float g, float b, float a = 1.0f) : ColorGuard(true, idx, r, g, b, a) {}

        ColorGuard(bool condition, ImGuiCol idx, const ImVec4& col) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleColor(idx, col);
        }

        ColorGuard(ImGuiCol idx, const ImVec4& col) : ColorGuard(true, idx, col) {}
        
        ColorGuard(bool condition, ImGuiCol idx, ImU32 col) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleColor(idx, col);
        }

        ColorGuard(ImGuiCol idx, ImU32 col) : ColorGuard(true, idx, col) {}

        ~ColorGuard() {
            if (m_pushed) {
                ImGui::PopStyleColor();
            }
        }
    };

    struct StyleGuard {
        bool m_pushed;

        StyleGuard(bool condition, ImGuiStyleVar idx, float val) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleVar(idx, val);
        }

        StyleGuard(ImGuiStyleVar idx, float val) : StyleGuard(true, idx, val) {}

        StyleGuard(bool condition, ImGuiStyleVar idx, const ImVec2& val) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleVar(idx, val);
        }

        StyleGuard(ImGuiStyleVar idx, const ImVec2& val) : StyleGuard(true, idx, val) {}

        ~StyleGuard() {
            if (m_pushed) {
                ImGui::PopStyleVar();
            }
        }
    };

    struct IDGuard {
        IDGuard(int idx) { ImGui::PushID(idx); }
        IDGuard(const char* str_id) { ImGui::PushID(str_id); }
        IDGuard(const void* ptr_id) { ImGui::PushID(ptr_id); }
        ~IDGuard() { ImGui::PopID(); }
    };

    struct ClipRectGuard {
        bool m_pushed;

        ClipRectGuard(bool condition, const ImVec2& min, const ImVec2& max, bool intersect = true) {
            m_pushed = condition;
            if (m_pushed) {
                ImGui::PushClipRect(min, max, intersect);
            }
        }

        ClipRectGuard(const ImVec2& min, const ImVec2& max, bool intersect = true) : ClipRectGuard(true, min, max, intersect) {}

        ~ClipRectGuard() {
            if (m_pushed) {
                ImGui::PopClipRect();
            }
        }
    };

} // namespace UI
} // namespace NexusSDK
