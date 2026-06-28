#pragma once
#include "nexus-sdk-resource.h"

// Core Managers
#include "managers/AudioManager.h"
#include "managers/ContentManager.h"
#include "managers/LocalManager.h"

// Services
#include "services/Gw2MumbleService.h"
#include "services/RealTimeApiService.h"
#include "services/NexusService.h"

// UI Framework
#include "ui/controls/ControlBase.h"
#include "ui/Viewport.h"
#include "ui/UIScale.h"
#include "ui/controls/Label.h"
#include "ui/controls/Container.h"
#include "ui/controls/Tooltip.h"
#include "ui/controls/WindowBase.h"
#include "ui/controls/Checkbox.h"
#include "ui/controls/Button.h"
#include "ui/controls/Slider.h"
#include "ui/controls/ComboBox.h"
#include "ui/controls/Dropdown.h"
#include "ui/controls/Menu.h"
#include "ui/controls/FlowPanel.h"
#include "ui/controls/StandardDialog.h"

// Utilities
#include "utils/AsyncFont.h"
#include "utils/AsyncTexture.h"
#include "utils/ImID.h"

#include <filesystem>
#include <windows.h>

namespace NexusSDK {
    inline AddonAPI_t* API = nullptr;
    inline HMODULE Module = nullptr;
    inline AudioManager* Audio = nullptr;
    inline ContentManager* Content = nullptr;
    inline LocalManager* Local = nullptr;
    inline LocalManager* SDKLocal = nullptr;
    inline Services::Gw2MumbleService* Mumble = nullptr;
    inline Services::RealTimeApiService* RTAPI = nullptr;
    inline Services::NexusService* Nexus = nullptr;

    inline UINT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg >= 0x0200 && uMsg <= 0x020E) {
            if (NexusSDK::UI::Viewport::Get()) {
                float xPos = (float)(short)(lParam & 0xFFFF);
                float yPos = (float)(short)((lParam >> 16) & 0xFFFF);
                
                auto capType = NexusSDK::UI::Viewport::Get()->HitTest(ImVec2(xPos, yPos));
                if (capType == NexusSDK::UI::CaptureType::Capture) {
                    return 0; // Block game
                }
            }
        }
        return uMsg;
    }

    inline void Initialize(AddonAPI_t* api, HMODULE hSelf, const std::filesystem::path& addonPath) {
        API = api;
        Module = hSelf;
        API->WndProc_Register(WndProc);
        NexusSDK::UI::Viewport::Initialize();
        if (!Audio) Audio = new AudioManager(api);
        if (!Content) Content = new ContentManager(api, hSelf);
        if (!Local && !addonPath.empty()) Local = new LocalManager(addonPath, api);
        if (!Mumble) Mumble = new Services::Gw2MumbleService(api);
        if (!RTAPI) RTAPI = new Services::RealTimeApiService(api);
        if (!Nexus) Nexus = new Services::NexusService(api);
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

        auto langProvider = []() -> int {
            if (RTAPI && RTAPI->Data()) {
                return RTAPI->Data()->Language;
            }
            return 0;
        };
        if (Local) Local->SetLanguageProvider(langProvider);
        if (SDKLocal) SDKLocal->SetLanguageProvider(langProvider);
    }

    inline void Shutdown() {
        if (API) {
            API->WndProc_Deregister(WndProc);
        }
        NexusSDK::UI::Viewport::Shutdown();
        if (Audio) { delete Audio; Audio = nullptr; }
        if (Content) { delete Content; Content = nullptr; }
        if (Local) { delete Local; Local = nullptr; }
        if (SDKLocal) { delete SDKLocal; SDKLocal = nullptr; }
        if (Mumble) { delete Mumble; Mumble = nullptr; }
        if (RTAPI) { delete RTAPI; RTAPI = nullptr; }
        if (Nexus) { delete Nexus; Nexus = nullptr; }
        API = nullptr;
        Module = nullptr;
    }

    inline void Render(float uiScale = -1.0f) {
        if (uiScale < 0.0f) {
            uiScale = 1.0f;
            if (Nexus && Nexus->Core()) {
                uiScale = Nexus->Core()->Scaling;
            }
        }
        NexusSDK::UI::UIScale::Set(uiScale);
        NexusSDK::UI::Viewport::Render(uiScale);
    }
}
