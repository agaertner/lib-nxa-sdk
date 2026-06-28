#ifndef ASYNCFONT_H
#define ASYNCFONT_H

#include <Nexus.h>
#include <string>
#include <map>

struct ImFont;

namespace NexusSDK {
    class AsyncFont {
    public:
        AsyncFont(const std::string& identifier, const std::string& resourceName, float fontSize, AddonAPI_t* api, HMODULE moduleHandle);
        AsyncFont(ImFont* unownedFont);
        ~AsyncFont();

        // Requests Nexus to load the font from the resource
        void Load();

        // Returns the ImFont* if it has been loaded and initialized
        ImFont* Get();

    private:
        std::string m_identifier;
        std::string m_resourceName;
        float m_fontSize;
        AddonAPI_t* m_api;
        HMODULE m_moduleHandle;

        bool m_isLoadRequested;
        bool m_isOwned;
        ImFont* m_unownedFont;

        static std::map<std::string, void*> s_loadedFonts;
        static void FontReceiveCallback(const char* identifier, void* font);
    };
}

#endif // ASYNCFONT_H

