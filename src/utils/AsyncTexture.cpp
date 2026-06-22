#include "AsyncTexture.h"
#include <windows.h>

namespace NexusSDK {

    AsyncTexture::AsyncTexture(const std::string& identifier, const std::string& resourceName, AddonAPI_t* api, HMODULE moduleHandle)
        : m_identifier(identifier), m_resourceName(resourceName),
          m_api(api), m_moduleHandle(moduleHandle), m_texture(nullptr), m_isLoadRequested(false)
    {
    }

    void AsyncTexture::Load() {
        if (m_isLoadRequested) return;

        HRSRC hRes = FindResourceA(m_moduleHandle, m_resourceName.c_str(), (LPCSTR)RT_RCDATA);

        if (hRes) {
            HGLOBAL hMem = LoadResource(m_moduleHandle, hRes);
            void* pData = LockResource(hMem);
            DWORD size = SizeofResource(m_moduleHandle, hRes);
            if (pData && size > 0) {
                m_texture = m_api->Textures_GetOrCreateFromMemory(m_identifier.c_str(), pData, size);
                m_isLoadRequested = true;
            }
        }
    }

    Texture_t* AsyncTexture::Get() {
        if (!m_isLoadRequested) {
            return nullptr;
        }

        if (!m_texture || !m_texture->Resource) {
            m_texture = m_api->Textures_Get(m_identifier.c_str());
        }

        return m_texture;
    }

    void AsyncTexture::Dispose() {
        m_texture = nullptr;
        m_isLoadRequested = false;
    }

}
