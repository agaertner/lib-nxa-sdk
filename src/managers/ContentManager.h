#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <windows.h>
struct AddonAPI_t;
#include <nexus-imgui/imgui.h>
#include "../utils/AsyncTexture.h"
#include "../utils/AsyncFont.h"

namespace NexusSDK {
    class ContentManager {
    public:
        ContentManager(AddonAPI_t* api, HMODULE moduleHandle);
        ~ContentManager();

        Texture_t* GetTexture(const std::string& resourceName);
        ImFont* GetFont(const std::string& resourceName, float fontSize);

        void Clear();

    private:
        AddonAPI_t* m_api;
        HMODULE m_moduleHandle;

        std::unordered_map<std::string, std::unique_ptr<AsyncTexture>> m_textures;
        std::unordered_map<std::string, std::unique_ptr<AsyncFont>> m_fonts;
    };
}
