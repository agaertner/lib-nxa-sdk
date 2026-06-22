#pragma once
#include "Defines.h"
#include <windows.h>
#include "../managers/AudioManager.h"

namespace NexusSDK {
namespace UI {
    extern AddonAPI_t* ContextAPI;
    extern HMODULE ContextModule;
    extern AudioManager* ContextAudio;
    
    void Initialize(AddonAPI_t* api, HMODULE moduleHandle, AudioManager* audio = nullptr);
}
}
