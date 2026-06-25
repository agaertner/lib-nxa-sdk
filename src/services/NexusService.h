#ifndef NEXUSSERVICE_H
#define NEXUSSERVICE_H
#include <Nexus.h>

struct ImFont;

namespace NexusSDK::Services
{
    class NexusService
    {
        public:
            static NexusService* Get() { return m_instance; }

            NexusService(AddonAPI_t* p_api);

            ~NexusService();

            NexusLinkData_t* Core() const;

            ImFont* Font() const; // Menomonia 18px
            ImFont* FontBig() const; // Menomonia 36px
            ImFont* FontUI() const; // Menomonia 16px

        private:
            inline static NexusService* m_instance = nullptr;

            AddonAPI_t* m_api;
            NexusLinkData_t* m_nexus;
    };
}
#endif
