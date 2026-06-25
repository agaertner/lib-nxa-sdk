#ifndef ASYNCFONT_H
#define ASYNCFONT_H

#include <Nexus.h>
#include <string>
#include <map>

namespace NexusSDK {
    class AsyncFont {
    public:
        AsyncFont(const std::string& identifier, const std::string& resourceName, float fontSize, AddonAPI_t* api, HMODULE moduleHandle);
        ~AsyncFont();

        // Requests Nexus to load the font from the resource
        void Load();

        // Returns the ImFont* if it has been loaded and initialized
        void* Get();

        // Releases the font from Nexus
        void Dispose();

    private:
        std::string m_identifier;
        std::string m_resourceName;
        float m_fontSize;
        AddonAPI_t* m_api;
        HMODULE m_moduleHandle;

        bool m_isLoadRequested;

        static std::map<std::string, void*> s_loadedFonts;
        static void FontReceiveCallback(const char* identifier, void* font);
    };
}

#endif // ASYNCFONT_H

