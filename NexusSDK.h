#pragma once
#include "nexus-sdk-resource.h"

// Core Managers
#include "src/managers/AudioManager.h"
#include "src/managers/ContentManager.h"
#include "src/managers/LocalManager.h"

// UI Framework
#include "src/ui/controls/ControlBase.h"
#include "src/ui/Viewport.h"
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
#include "src/ui/controls/StandardDialog.h"

// Utilities
#include "src/utils/AsyncFont.h"
#include "src/utils/AsyncTexture.h"
#include "src/utils/ImID.h"

#include <filesystem>
#include <windows.h>

namespace NexusSDK {
    inline AddonAPI_t* API = nullptr;
    inline HMODULE Module = nullptr;
    inline AudioManager* Audio = nullptr;
    inline ContentManager* Content = nullptr;
    inline LocalManager* Local = nullptr;
    inline LocalManager* SDKLocal = nullptr;

    inline void Initialize(AddonAPI_t* api, HMODULE hSelf, const std::filesystem::path& addonPath) {
        API = api;
        Module = hSelf;
        NexusSDK::UI::Viewport::Initialize();
        if (!Audio) Audio = new AudioManager(api);
        if (!Content) Content = new ContentManager(api, hSelf);
        if (!Local && !addonPath.empty()) Local = new LocalManager(addonPath, api);
        if (!SDKLocal) {
            SDKLocal = new LocalManager("", api);
            HRSRC hRes = FindResourceA(hSelf, "NXA_SDK_LOCALIZATION", (LPCSTR)10); // RT_RCDATA is 10
            if (hRes) {
                HGLOBAL hData = LoadResource(hSelf, hRes);
                if (hData) {
                    DWORD dataSize = SizeofResource(hSelf, hRes);
                    void* pData = LockResource(hData);
                    if (pData && dataSize > 0) {
                        std::string jsonStr(static_cast<const char*>(pData), dataSize);
                        SDKLocal->LoadFromMemory(jsonStr);
                    }
                }
            }
        }
    }

    inline void Shutdown() {
        NexusSDK::UI::Viewport::Shutdown();
        if (Audio) { delete Audio; Audio = nullptr; }
        if (Content) { delete Content; Content = nullptr; }
        if (Local) { delete Local; Local = nullptr; }
        if (SDKLocal) { delete SDKLocal; SDKLocal = nullptr; }
        API = nullptr;
        Module = nullptr;
    }

    inline void Render(float uiScale = 1.0f) {
        NexusSDK::UI::Viewport::Render(uiScale);
    }
}
