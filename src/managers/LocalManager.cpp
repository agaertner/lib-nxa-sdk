#include "LocalManager.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace NexusSDK {

    LocalManager::LocalManager(const std::filesystem::path& addonPath, AddonAPI_t* api)
        : m_api(api)
    {
        m_filePath = addonPath / "localization.json";
        Load(m_filePath);
    }

    void LocalManager::SetLanguageProvider(std::function<int()> provider) {
        m_languageProvider = provider;
    }

    void LocalManager::Load(const std::filesystem::path& filePath)
    {
        if (!std::filesystem::exists(filePath)) {
            CreateDefaultFile(filePath);
        }

        try {
            std::ifstream file(filePath);
            json j;
            file >> j;

            for (auto& el : j.items()) {
                std::string key = el.key();
                for (auto& langEl : el.value().items()) {
                    std::string langId = langEl.key();
                    if (langEl.value().is_string()) {
                        m_strings[key][langId] = langEl.value().get<std::string>();
                    }
                }
            }
        }
        catch (const json::exception& e) {
            if (m_api) {
                m_api->Log(ELogLevel::LOGL_WARNING, "LocalManager", "localization.json could not be parsed.");
                m_api->Log(ELogLevel::LOGL_WARNING, "LocalManager", e.what());
            }
        }
    }

    void LocalManager::CreateDefaultFile(const std::filesystem::path& filePath)
    {
        json defaultJson = {
            {"Example_Text", {
                {"en", "Example Text"},
                {"de", "Beispieltext"}
            }}
        };

        try {
            std::ofstream file(filePath);
            file << defaultJson.dump(4);
        }
        catch (const std::exception&) {
            if (m_api) {
                m_api->Log(ELogLevel::LOGL_WARNING, "LocalManager", "Failed to create default localization.json.");
            }
        }
    }

    std::string LocalManager::GetIsoCode(int langId)
    {
        switch (langId) {
            case 2: return "fr";
            case 3: return "de";
            case 4: return "es";
            case 5: return "zh";
            default: return "en";
        }
    }

    std::string LocalManager::GetString(const std::string& key, std::optional<int> language)
    {
        int langId = 0; // Default English

        if (language.has_value()) {
            langId = language.value();
        } else {
            if (m_languageProvider) {
                langId = m_languageProvider();
            }
        }

        std::string langStr = GetIsoCode(langId);

        auto keyIt = m_strings.find(key);
        if (keyIt != m_strings.end()) {
            auto langIt = keyIt->second.find(langStr);
            if (langIt != keyIt->second.end()) {
                return langIt->second;
            }

            // Fallback to English ("en")
            auto enIt = keyIt->second.find("en");
            if (enIt != keyIt->second.end()) {
                return enIt->second;
            }
        }

        return key;
    }

    std::string LocalManager::GetString(const std::string& key, const std::vector<std::string>& args, std::optional<int> language)
    {
        std::string str = GetString(key, language);
        for (size_t i = 0; i < args.size(); ++i) {
            std::string placeholder = "{" + std::to_string(i) + "}";
            size_t pos = 0;
            while ((pos = str.find(placeholder, pos)) != std::string::npos) {
                str.replace(pos, placeholder.length(), args[i]);
                pos += args[i].length();
            }
        }
        return str;
    }

}
