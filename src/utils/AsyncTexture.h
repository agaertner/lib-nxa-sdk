#ifndef ASYNCTEXTURE_H
#define ASYNCTEXTURE_H

#include "Defines.h"
#include <string>

namespace NexusSDK {
    class AsyncTexture {
    public:
        AsyncTexture(const std::string& identifier, int resourceID, AddonAPI_t* api, HMODULE moduleHandle);
        ~AsyncTexture() = default;

        // Triggers the extraction of the resource and requests Nexus to load it
        void Load();

        // Fetches the texture, polling Nexus if it was loading asynchronously
        Texture_t* Get();

        // Disposes the texture reference
        void Dispose();

    private:
        std::string m_identifier;
        int m_resourceID;
        AddonAPI_t* m_api;
        HMODULE m_moduleHandle;

        Texture_t* m_texture;
        bool m_isLoadRequested;
    };
}

#endif // ASYNCTEXTURE_H
