#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace NexusSDK {
namespace UI {

class ImID {
public:
    static std::string Create(const std::string& className) {
        static std::unordered_map<std::string, uint64_t> s_counters;
        static std::mutex s_mutex;
        
        uint64_t count = 0;
        {
            std::lock_guard<std::mutex> lock(s_mutex);
            count = ++s_counters[className];
        }
        
        std::stringstream ss;
        ss << "##" << className << "_" << std::setfill('0') << std::setw(5) << count;
        return ss.str();
    }
};

} // namespace UI
} // namespace NexusSDK
