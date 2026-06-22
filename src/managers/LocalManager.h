#ifndef LOCAL_MANAGER_H
#define LOCAL_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <filesystem>
#include "thirdparty/nlohmann/json.hpp"
#include <functional>
#include "Defines.h"

namespace NexusSDK {

    class LocalManager {
    public:
        LocalManager(const std::filesystem::path& addonPath, AddonAPI_t* api = nullptr);
        ~LocalManager() = default;

        void SetLanguageProvider(std::function<int()> provider);

        void SetDefault(const std::string& defaultJsonString);
        void Save();
        void LoadFromMemory(const std::string& jsonString);

        std::string GetString(const std::string& key, std::optional<int> language = std::nullopt);
        std::string GetString(const std::string& key, const std::vector<std::string>& args, std::optional<int> language = std::nullopt);
        std::string GetIsoCode(int langId);

    private:
        void Load(const std::filesystem::path& filePath);
        void CreateDefaultFile(const std::filesystem::path& filePath);

        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_strings;
        std::filesystem::path m_filePath;
        AddonAPI_t* m_api;
        std::function<int()> m_languageProvider;
    };

}

#endif // LOCAL_MANAGER_H
