#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <windows.h>
#include <xaudio2.h>
#include <mutex>
#include <unordered_map>
#include <cstdint>
#include "Defines.h"

namespace NexusSDK {
    class AudioManager;
    struct AudioCacheEntry {
        WAVEFORMATEX* format;
        const BYTE* data;
        DWORD dataSize;
        UINT32 playBegin;
    };

    class AudioHandle {
        uint64_t m_id;
        AudioManager* m_manager;
    public:
        AudioHandle() : m_id(0), m_manager(nullptr) {}
        AudioHandle(uint64_t id, AudioManager* mgr) : m_id(id), m_manager(mgr) {}
        ~AudioHandle() = default;
        
        AudioHandle(AudioHandle&& other) noexcept : m_id(other.m_id), m_manager(other.m_manager) {
            other.m_id = 0;
            other.m_manager = nullptr;
        }
        AudioHandle& operator=(AudioHandle&& other) noexcept;

        AudioHandle(const AudioHandle&) = delete;
        AudioHandle& operator=(const AudioHandle&) = delete;

        void Stop();
        uint64_t GetId() const { return m_id; }
    };



    class AudioManager {
    public:
        AudioManager(AddonAPI_t* api);
        ~AudioManager();

        AudioHandle Play(const std::string& resourceName, const std::string& channel = "UI", bool loop = false);
        void Stop(uint64_t voiceId);
        void StopAll();
        void SetMasterVolume(float volume);
        void SetChannelVolume(const std::string& channel, float volume);
        float GetChannelVolume(const std::string& channel);

    private:
        AddonAPI_t* m_api;
        IXAudio2* m_pXAudio2;
        IXAudio2MasteringVoice* m_pMasterVoice;
        bool m_initialized;
        bool m_coInitialized;
        float m_masterVolume = 1.0f;
        std::unordered_map<std::string, float> m_channelVolumes;
        std::mutex m_mutex;
        
        std::unordered_map<std::string, AudioCacheEntry> m_cache;
        uint64_t m_nextVoiceId = 1;

        struct ActiveVoice {
            IXAudio2SourceVoice* voice;
            std::string channel;
        };

        std::unordered_map<uint64_t, ActiveVoice> m_activeVoices;

        AudioHandle PlayFromEntry(AudioCacheEntry& entry);
        bool FindChunk(const BYTE* data, size_t size, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition);
        UINT32 TrimSilence(WAVEFORMATEX* format, const BYTE* data, DWORD size);
    };
}

#endif
