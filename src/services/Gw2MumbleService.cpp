#include "Gw2MumbleService.h"

NexusSDK::Services::Gw2MumbleService::Gw2MumbleService(AddonAPI_t* p_api)
    : m_api(p_api),
    m_link((Mumble::Data*)p_api->DataLink_Get("DL_MUMBLE_LINK")),
    m_identity()

{
    m_instance = this;
    m_api->Events_Subscribe(MUMBLE_IDENTITY_UPDATED, OnMumbleIdentityUpdated);
}
NexusSDK::Services::Gw2MumbleService::~Gw2MumbleService()
{
    m_api->Events_Unsubscribe(MUMBLE_IDENTITY_UPDATED, OnMumbleIdentityUpdated);
    m_instance = nullptr;
}

Mumble::Data* NexusSDK::Services::Gw2MumbleService::Data() const
{
    return m_link;
}

Mumble::Identity* NexusSDK::Services::Gw2MumbleService::Identity() const
{
    return m_identity;
}

bool NexusSDK::Services::Gw2MumbleService::IsActive() const
{
    if (!m_link || m_link->Context.BuildID == 0) return false;

    unsigned currentTick = m_link->UITick;
    auto currentTime = std::chrono::steady_clock::now();

    if (currentTick != m_lastUiTick) {
        m_lastUiTick = currentTick;
        m_lastUiTickTime = currentTime;
        return true;
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastUiTickTime).count();
    if (elapsed > 500) {
        return false;
    }

    return true;
}

float NexusSDK::Services::Gw2MumbleService::GetUIScale() const
{
    if (!m_identity) return 1.0f;
    
    switch (m_identity->UISize) {
        case Mumble::EUIScale::Small: return 0.897f;
        case Mumble::EUIScale::Normal: return 1.0f;
        case Mumble::EUIScale::Large: return 1.111f;
        case Mumble::EUIScale::Larger: return 1.222f;
        default: return 1.0f;
    }
}