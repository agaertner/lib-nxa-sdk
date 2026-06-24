#ifndef REALTIMEAPISERVICE_H
#define REALTIMEAPISERVICE_H
#include <nexus-core/Nexus.h>
#include "../../nexus-rtapi/RTAPI.h"

namespace NexusSDK::Services
{
    class RealTimeApiService
    {
        const char* ADDON_LOADED = "EV_ADDON_LOADED";
        const char* ADDON_UNLOADED = "EV_ADDON_UNLOADED";
        public:
            RealTimeApiService(AddonAPI_t* p_api);
            ~RealTimeApiService();

            RealTimeData* Data() const;
            bool IsAvailable() const;

        private:
            inline static RealTimeApiService* m_instance = nullptr;

            AddonAPI_t* m_api;
            RealTimeData* m_rtdata;

            static void OnAddonLoaded(void* aSignature)
            {
                if (m_instance)
                {
                    int* sig = static_cast<int*>(aSignature);
                    if (!sig) { return; }
                    
                    // Initialize real time API.
                    if (*sig == RTAPI_SIG) m_instance->m_rtdata = (RealTimeData*)m_instance->m_api->DataLink_Get(DL_RTAPI);
                }
            }

            static void OnAddonUnloaded(void* aSignature)
            {
                if (m_instance)
                {
                    int* sig = static_cast<int*>(aSignature);
                    if (!sig) { return; }

                    // Unset real time API.
                    if (*sig == RTAPI_SIG) m_instance->m_rtdata = nullptr;
                }
            }
    };
}
#endif

