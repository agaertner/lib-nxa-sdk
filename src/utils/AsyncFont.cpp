#include "AsyncFont.h"
#include <windows.h>
#include "imgui/imgui.h"

namespace NexusSDK {

    std::map<std::string, void*> AsyncFont::s_loadedFonts;

    void AsyncFont::FontReceiveCallback(const char* identifier, void* font) {
        if (identifier && font) {
            s_loadedFonts[identifier] = font;
        }
    }

    AsyncFont::AsyncFont(const std::string& identifier, int resourceID, float fontSize, AddonAPI_t* api, HMODULE moduleHandle)
        : m_identifier(identifier), m_resourceID(resourceID), m_fontSize(fontSize), m_api(api), m_moduleHandle(moduleHandle), m_isLoadRequested(false)
    {
    }

    AsyncFont::~AsyncFont() {
        Dispose();
    }

    void AsyncFont::Load() {
        if (m_isLoadRequested) return;

        if (s_loadedFonts.find(m_identifier) != s_loadedFonts.end()) {
            m_isLoadRequested = true;
            return;
        }

        HRSRC hResource = FindResource(m_moduleHandle, MAKEINTRESOURCE(m_resourceID), RT_RCDATA);
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

    void* AsyncFont::Get() {
        if (!m_isLoadRequested) return nullptr;

        auto it = s_loadedFonts.find(m_identifier);
        if (it != s_loadedFonts.end()) {
            return it->second;
        }
        return nullptr;
    }

    void AsyncFont::Dispose() {
        if (m_isLoadRequested) {
            m_api->Fonts_Release(m_identifier.c_str(), FontReceiveCallback);
            s_loadedFonts.erase(m_identifier);
            m_isLoadRequested = false;
        }
    }

}
