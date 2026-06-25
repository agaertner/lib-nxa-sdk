#include "NexusService.h"
#include <imgui.h>

NexusSDK::Services::NexusService::NexusService(AddonAPI_t* p_api) : m_api(p_api)
{
    m_instance = this;
    m_nexus = (NexusLinkData_t*)m_api->DataLink_Get("DL_NEXUS_LINK");
}

NexusSDK::Services::NexusService::~NexusService()
{
    m_instance = nullptr;
}

NexusLinkData_t* NexusSDK::Services::NexusService::Core() const
{
    return m_nexus;
}

ImFont* NexusSDK::Services::NexusService::Font() const
{
    return m_nexus ? (ImFont*)m_nexus->Font : nullptr;
}

ImFont* NexusSDK::Services::NexusService::FontBig() const
{
    return m_nexus ? (ImFont*)m_nexus->FontBig : nullptr;
}

ImFont* NexusSDK::Services::NexusService::FontUI() const
{
    return m_nexus ? (ImFont*)m_nexus->FontUI : nullptr;
}