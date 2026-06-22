#pragma once

// Core Managers
#include "src/managers/AudioManager.h"
#include "src/managers/ContentManager.h"
#include "src/managers/LocalManager.h"

// UI Framework
#include "src/ui/controls/ControlBase.h"
#include "src/ui/controls/Label.h"
#include "src/ui/controls/Container.h"
#include "src/ui/controls/Tooltip.h"
#include "src/ui/controls/WindowBase.h"
#include "src/ui/controls/Checkbox.h"
#include "src/ui/controls/Slider.h"
#include "src/ui/controls/ComboBox.h"
#include "src/ui/controls/Dropdown.h"
#include "src/ui/controls/Menu.h"
#include "src/ui/controls/FlowPanel.h"

// Utilities
#include "src/utils/AsyncFont.h"
#include "src/utils/AsyncTexture.h"
#include "src/utils/ImID.h"

#include <filesystem>

namespace NexusSDK {
    inline AddonAPI_t* API = nullptr;
    inline HMODULE Module = nullptr;
    inline AudioManager* Audio = nullptr;
    inline ContentManager* Content = nullptr;
    inline LocalManager* Local = nullptr;

    inline void Initialize(AddonAPI_t* api, HMODULE hSelf, const std::filesystem::path& addonPath) {
        API = api;
        Module = hSelf;
        if (!Audio) Audio = new AudioManager(api);
        if (!Content) Content = new ContentManager(api, hSelf);
        if (!Local && !addonPath.empty()) Local = new LocalManager(addonPath, api);
    }

    inline void Shutdown() {
        if (Audio) { delete Audio; Audio = nullptr; }
        if (Content) { delete Content; Content = nullptr; }
        if (Local) { delete Local; Local = nullptr; }
        API = nullptr;
        Module = nullptr;
    }
}
