#pragma once
#include <nexus-imgui/imgui.h>

namespace NexusSDK {
namespace UI {

    struct Font {
        bool m_pushed;

        Font(ImFont* font) {
            m_pushed = font != nullptr;
            if (m_pushed) {
                ImGui::PushFont(font);
            }
        }

        ~Font() {
            if (m_pushed) {
                ImGui::PopFont();
            }
        }
    };

    struct Color {
        bool m_pushed;

        Color(bool condition, float r, float g, float b, float a = 1.0f) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, a));
        }

        Color(float r, float g, float b, float a = 1.0f) : Color(true, r, g, b, a) {}

        Color(bool condition, ImGuiCol idx, float r, float g, float b, float a = 1.0f) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleColor(idx, ImVec4(r, g, b, a));
        }

        Color(ImGuiCol idx, float r, float g, float b, float a = 1.0f) : Color(true, idx, r, g, b, a) {}

        Color(bool condition, ImGuiCol idx, const ImVec4& col) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleColor(idx, col);
        }

        Color(ImGuiCol idx, const ImVec4& col) : Color(true, idx, col) {}
        
        Color(bool condition, ImGuiCol idx, ImU32 col) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleColor(idx, col);
        }

        Color(ImGuiCol idx, ImU32 col) : Color(true, idx, col) {}

        ~Color() {
            if (m_pushed) {
                ImGui::PopStyleColor();
            }
        }
    };

    struct Style {
        bool m_pushed;

        Style(bool condition, ImGuiStyleVar idx, float val) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleVar(idx, val);
        }

        Style(ImGuiStyleVar idx, float val) : Style(true, idx, val) {}

        Style(bool condition, ImGuiStyleVar idx, const ImVec2& val) {
            m_pushed = condition;
            if (m_pushed) ImGui::PushStyleVar(idx, val);
        }

        Style(ImGuiStyleVar idx, const ImVec2& val) : Style(true, idx, val) {}

        ~Style() {
            if (m_pushed) {
                ImGui::PopStyleVar();
            }
        }
    };

    struct ID {
        ID(int idx) { ImGui::PushID(idx); }
        ID(const char* str_id) { ImGui::PushID(str_id); }
        ID(const void* ptr_id) { ImGui::PushID(ptr_id); }
        ~ID() { ImGui::PopID(); }
    };

} // namespace UI
} // namespace NexusSDK
