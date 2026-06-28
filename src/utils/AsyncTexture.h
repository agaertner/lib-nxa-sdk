#ifndef ASYNCTEXTURE_H
#define ASYNCTEXTURE_H

#include <Nexus.h>
#include <string>

namespace NexusSDK {
    class AsyncTexture {
    public:
        AsyncTexture(const std::string& identifier, const std::string& resourceName, AddonAPI_t* api, HMODULE moduleHandle);
        AsyncTexture(Texture_t* unownedTexture);
        ~AsyncTexture();

        void Load();
        Texture_t* Get();
        int GetWidth();
        int GetHeight();

    private:
        std::string m_identifier;
        std::string m_resourceName;
        AddonAPI_t* m_api;
        HMODULE m_moduleHandle;

        Texture_t* m_texture;
        bool m_isLoadRequested;
        bool m_isOwned;
    };
}

#endif // ASYNCTEXTURE_H

