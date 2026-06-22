#include "ContentManager.h"

namespace NexusSDK {

    ContentManager::ContentManager(AddonAPI_t* api, HMODULE moduleHandle)
        : m_api(api), m_moduleHandle(moduleHandle)
    {
    }

    ContentManager::~ContentManager() {
        Clear();
    }

    Texture_t* ContentManager::GetTexture(const std::string& resourceName) {
        auto it = m_textures.find(resourceName);
        if (it == m_textures.end()) {
            // Identifier and ResourceName are the same
            auto tex = std::make_unique<AsyncTexture>(resourceName, resourceName, m_api, m_moduleHandle);
            tex->Load();
            m_textures[resourceName] = std::move(tex);
            return nullptr; // Will be available on subsequent calls
        }
        return it->second->Get();
    }

    ImFont* ContentManager::GetFont(const std::string& resourceName, float fontSize) {
        // Construct a unique identifier combining resource name and size
        std::string identifier = resourceName + "_" + std::to_string(static_cast<int>(fontSize));
        
        auto it = m_fonts.find(identifier);
        if (it == m_fonts.end()) {
            auto font = std::make_unique<AsyncFont>(identifier, resourceName, fontSize, m_api, m_moduleHandle);
            font->Load();
            m_fonts[identifier] = std::move(font);
            return nullptr;
        }
        return (ImFont*)it->second->Get();
    }

    void ContentManager::Clear() {
        m_textures.clear();
        m_fonts.clear();
    }

}
