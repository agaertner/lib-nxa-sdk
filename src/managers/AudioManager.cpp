#include "AudioManager.h"


#pragma comment(lib, "xaudio2.lib")

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

namespace NexusSDK {

    static HMODULE GetCurrentModuleHandle() {
        HMODULE hModule = NULL;
        GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)GetCurrentModuleHandle,
            &hModule);
        return hModule;
    }

    AudioManager::AudioManager(AddonAPI_t* api) : m_api(api), m_pXAudio2(nullptr), m_pMasterVoice(nullptr), m_initialized(false), m_coInitialized(false) {
        HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        m_coInitialized = SUCCEEDED(hrInit);
        
        HRESULT hr = XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr)) return;

        hr = m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice);
        if (FAILED(hr)) {
            m_pXAudio2->Release();
            m_pXAudio2 = nullptr;
            return;
        }

        m_initialized = true;
    }

    AudioManager::~AudioManager() {
        StopAll();
        if (m_pMasterVoice) {
            m_pMasterVoice->DestroyVoice();
            m_pMasterVoice = nullptr;
        }
        if (m_pXAudio2) {
            m_pXAudio2->Release();
            m_pXAudio2 = nullptr;
        }
        if (m_coInitialized) {
            CoUninitialize();
        }
    }

    bool AudioManager::FindChunk(const BYTE* data, size_t size, DWORD fourcc, DWORD& chunkSize, DWORD& chunkDataPosition) {
        DWORD offset = 0;
        while (offset + 8 <= size) {
            DWORD chunkType = *reinterpret_cast<const DWORD*>(data + offset);
            DWORD chunkDataSize = *reinterpret_cast<const DWORD*>(data + offset + 4);
            if (chunkType == fourcc) {
                chunkSize = chunkDataSize;
                chunkDataPosition = offset + 8;
                return true;
            }
            offset += 8 + ((chunkDataSize + 1) & ~1);
        }
        return false;
    }

    AudioHandle& AudioHandle::operator=(AudioHandle&& other) noexcept {
        if (this != &other) {
            m_id = other.m_id;
            m_manager = other.m_manager;
            other.m_id = 0;
        }
        return *this;
    }

    void AudioHandle::Stop() {
        if (m_id != 0 && m_manager) {
            m_manager->Stop(m_id);
            m_id = 0;
        }
    }

    void AudioManager::StopAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& pair : m_activeVoices) {
            pair.second->Stop(0);
            pair.second->DestroyVoice();
        }
        m_activeVoices.clear();
    }

    void AudioManager::SetVolume(float volume) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_masterVolume = volume;
        for (auto& pair : m_activeVoices) {
            pair.second->SetVolume(m_masterVolume);
        }
    }

    void AudioManager::Stop(uint64_t voiceId) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_activeVoices.find(voiceId);
        if (it != m_activeVoices.end()) {
            it->second->Stop(0);
            it->second->DestroyVoice();
            m_activeVoices.erase(it);
        }
    }



    UINT32 AudioManager::TrimSilence(WAVEFORMATEX* format, const BYTE* data, DWORD size) {
        if (!format || !data || size == 0) return 0;
        
        if (format->wFormatTag != WAVE_FORMAT_PCM) return 0;
        
        UINT32 sampleCount = size / (format->wBitsPerSample / 8);
        UINT32 channels = format->nChannels;
        
        if (format->wBitsPerSample == 16) {
            const short* pcm = reinterpret_cast<const short*>(data);
            for (UINT32 i = 0; i < sampleCount; i++) {
                if (std::abs(pcm[i]) > 50) {
                    return i / channels;
                }
            }
        } else if (format->wBitsPerSample == 8) {
            const BYTE* pcm = data;
            for (UINT32 i = 0; i < sampleCount; i++) {
                if (std::abs(pcm[i] - 128) > 5) {
                    return i / channels;
                }
            }
        }
        return 0;
    }

    AudioHandle AudioManager::Play(const std::string& resourceName, bool loop) {
        if (!m_initialized) return AudioHandle(0, this);
        std::lock_guard<std::mutex> lock(m_mutex);

        // Clean up finished voices
        for (auto it = m_activeVoices.begin(); it != m_activeVoices.end(); ) {
            XAUDIO2_VOICE_STATE state;
            it->second->GetState(&state);
            if (state.BuffersQueued == 0) {
                it->second->DestroyVoice();
                it = m_activeVoices.erase(it);
            } else {
                ++it;
            }
        }

        AudioCacheEntry entry = {};
        auto cacheIt = m_cache.find(resourceName);
        if (cacheIt != m_cache.end()) {
            entry = cacheIt->second;
        } else {
            HMODULE hModule = GetCurrentModuleHandle();
            if (!hModule) return AudioHandle(0, this);

            HRSRC hRes = FindResourceA(hModule, resourceName.c_str(), "WAVE");
            if (!hRes) return AudioHandle(0, this);

            HGLOBAL hMem = LoadResource(hModule, hRes);
            if (!hMem) return AudioHandle(0, this);

            const BYTE* data = static_cast<const BYTE*>(LockResource(hMem));
            DWORD size = SizeofResource(hModule, hRes);
            if (!data || size == 0) return AudioHandle(0, this);

            DWORD chunkSize = 0;
            DWORD chunkPosition = 0;
            if (!FindChunk(data, size, fourccRIFF, chunkSize, chunkPosition)) return AudioHandle(0, this);

            DWORD fileType = *reinterpret_cast<const DWORD*>(data + chunkPosition);
            if (fileType != fourccWAVE) return AudioHandle(0, this);

            const BYTE* riffBody = data + chunkPosition + 4;
            const size_t riffBodySize = chunkSize - 4;

            DWORD fmtChunkSize = 0;
            DWORD fmtChunkPosition = 0;
            if (!FindChunk(riffBody, riffBodySize, fourccFMT, fmtChunkSize, fmtChunkPosition)) return AudioHandle(0, this);

            WAVEFORMATEX* pwfx = (WAVEFORMATEX*)(riffBody + fmtChunkPosition);

            DWORD dataChunkSize = 0;
            DWORD dataChunkPosition = 0;
            if (!FindChunk(riffBody, riffBodySize, fourccDATA, dataChunkSize, dataChunkPosition)) return AudioHandle(0, this);

            entry.format = pwfx;
            entry.data = riffBody + dataChunkPosition;
            entry.dataSize = dataChunkSize;
            entry.playBegin = TrimSilence(pwfx, entry.data, entry.dataSize);

            m_cache[resourceName] = entry;
        }

        IXAudio2SourceVoice* pSourceVoice;
        HRESULT hr = m_pXAudio2->CreateSourceVoice(&pSourceVoice, entry.format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr);
        if (FAILED(hr)) return AudioHandle(0, this);

        XAUDIO2_BUFFER buffer = { 0 };
        buffer.AudioBytes = entry.dataSize;
        buffer.pAudioData = entry.data;
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.PlayBegin = entry.playBegin;
        if (loop) {
            buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
        }

        hr = pSourceVoice->SubmitSourceBuffer(&buffer);
        if (FAILED(hr)) {
            pSourceVoice->DestroyVoice();
            return AudioHandle(0, this);
        }

        pSourceVoice->SetVolume(m_masterVolume);
        pSourceVoice->Start(0);
        
        uint64_t id = m_nextVoiceId++;
        m_activeVoices[id] = pSourceVoice;
        return AudioHandle(id, this);
    }
}
