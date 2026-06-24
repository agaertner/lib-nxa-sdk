#include "RealTimeApiService.h"

NexusSDK::Services::RealTimeApiService::RealTimeApiService(AddonAPI_t* p_api) : m_api(p_api)
{
    m_instance = this;
    m_rtdata = (RealTimeData*)m_api->DataLink_Get(DL_RTAPI);

    m_api->Events_Subscribe(ADDON_LOADED, OnAddonLoaded);
    m_api->Events_Subscribe(ADDON_UNLOADED, OnAddonUnloaded);
}

NexusSDK::Services::RealTimeApiService::~RealTimeApiService()
{
    m_instance = nullptr;
    m_api->Events_Unsubscribe(ADDON_LOADED, OnAddonLoaded);
    m_api->Events_Unsubscribe(ADDON_UNLOADED, OnAddonUnloaded);
}

RealTimeData* NexusSDK::Services::RealTimeApiService::Data() const
{
    return m_rtdata;
}

bool NexusSDK::Services::RealTimeApiService::IsAvailable() const
{
    return m_rtdata != nullptr && m_rtdata->GameBuild != 0;
}
