#include "UIContext.h"

namespace NexusSDK {
namespace UI {
    AddonAPI_t* ContextAPI = nullptr;
    HMODULE ContextModule = nullptr;
    AudioManager* ContextAudio = nullptr;

    void Initialize(AddonAPI_t* api, HMODULE moduleHandle, AudioManager* audio) {
        ContextAPI = api;
        ContextModule = moduleHandle;
        ContextAudio = audio;
    }
}
}
