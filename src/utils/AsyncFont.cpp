#include "AsyncFont.h"
#include <windows.h>
#include <imgui.h>

namespace NexusSDK {

    std::map<std::string, void*> AsyncFont::s_loadedFonts;

    void AsyncFont::FontReceiveCallback(const char* identifier, void* font) {
        if (identifier && font) {
            s_loadedFonts[identifier] = font;
        }
    }

    AsyncFont::AsyncFont(const std::string& identifier, const std::string& resourceName, float fontSize, AddonAPI_t* api, HMODULE moduleHandle)
        : m_identifier(identifier), m_resourceName(resourceName), m_fontSize(fontSize), m_api(api), m_moduleHandle(moduleHandle), m_isLoadRequested(false), m_isOwned(true), m_unownedFont(nullptr)
    {
    }

    AsyncFont::AsyncFont(ImFont* unownedFont)
        : m_identifier(""), m_resourceName(""), m_fontSize(0.0f), m_api(nullptr), m_moduleHandle(nullptr), m_isLoadRequested(true), m_isOwned(false), m_unownedFont(unownedFont)
    {
    }

    AsyncFont::~AsyncFont() {
        if (m_isOwned && m_isLoadRequested) {
            m_api->Fonts_Release(m_identifier.c_str(), FontReceiveCallback);
            s_loadedFonts.erase(m_identifier);
            m_isLoadRequested = false;
        }
    }

    void AsyncFont::Load() {
        if (!m_isOwned || m_isLoadRequested) return;

        if (s_loadedFonts.find(m_identifier) != s_loadedFonts.end()) {
            m_isLoadRequested = true;
            return;
        }

        HRSRC hResource = FindResourceA(m_moduleHandle, m_resourceName.c_str(), (LPCSTR)RT_RCDATA);
        if (hResource) {
            HGLOBAL hMemory = LoadResource(m_moduleHandle, hResource);
            if (hMemory) {
                void* data = LockResource(hMemory);
                DWORD size = SizeofResource(m_moduleHandle, hResource);
                
                void* copiedData = ImGui::MemAlloc(size);
                memcpy(copiedData, data, size);
                
                m_api->Fonts_AddFromMemory(m_identifier.c_str(), m_fontSize, copiedData, size, FontReceiveCallback, nullptr);
            }
        }
        
        m_isLoadRequested = true;
    }

    ImFont* AsyncFont::Get() {
        if (!m_isOwned) return m_unownedFont;

        if (!m_isLoadRequested) {
            Load();
        }

        auto it = s_loadedFonts.find(m_identifier);
        if (it != s_loadedFonts.end()) {
            return (ImFont*)it->second;
        }
        return nullptr;
    }



}
