#include "AudioOutput.h"

// The project defines WIN32_LEAN_AND_MEAN, so <windows.h> does not pull in the multimedia
// headers that define WAVEFORMATEX / WAVE_FORMAT_IEEE_FLOAT, which <dsound.h> needs. Include
// <mmsystem.h> explicitly first (and in this order).
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

namespace ojgl {

// 2 channels * 4 bytes per float sample.
static constexpr unsigned long kBytesPerFrame = 2 * sizeof(float);

AudioOutput::~AudioOutput()
{
    close();
}

void AudioOutput::init(const float* interleavedStereo, unsigned long numFrames, unsigned long sampleRate, void* hWnd)
{
    close();
    _numFrames = numFrames;

    LPDIRECTSOUND8 ds = nullptr;
    if (FAILED(DirectSoundCreate8(nullptr, &ds, nullptr)))
        return;
    _dsound = ds;

    // A window is required to set the cooperative level; the foreground window is fine.
    ds->SetCooperativeLevel(hWnd ? static_cast<HWND>(hWnd) : GetForegroundWindow(), DSSCL_PRIORITY);

    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = sampleRate;
    wfx.wBitsPerSample = 32;
    wfx.nBlockAlign = static_cast<WORD>(kBytesPerFrame);
    wfx.nAvgBytesPerSec = sampleRate * kBytesPerFrame;
    wfx.cbSize = 0;

    DSBUFFERDESC desc = {};
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME;
    desc.dwBufferBytes = numFrames * kBytesPerFrame;
    desc.lpwfxFormat = &wfx;

    LPDIRECTSOUNDBUFFER buffer = nullptr;
    if (FAILED(ds->CreateSoundBuffer(&desc, &buffer, nullptr)))
        return;
    _buffer = buffer;

    // Copy the whole pre-rendered song into the buffer. For a fresh buffer of exactly this size
    // the lock is not split, so only the first segment is used.
    void* ptr1 = nullptr;
    void* ptr2 = nullptr;
    DWORD len1 = 0;
    DWORD len2 = 0;
    if (SUCCEEDED(buffer->Lock(0, desc.dwBufferBytes, &ptr1, &len1, &ptr2, &len2, 0))) {
        memcpy(ptr1, interleavedStereo, len1);
        if (ptr2 && len2)
            memcpy(ptr2, reinterpret_cast<const char*>(interleavedStereo) + len1, len2);
        buffer->Unlock(ptr1, len1, ptr2, len2);
    }
}

void AudioOutput::play(unsigned long startFrame)
{
    if (!_buffer)
        return;
    auto* buffer = static_cast<LPDIRECTSOUNDBUFFER>(_buffer);
    buffer->SetCurrentPosition(startFrame * kBytesPerFrame);
    buffer->Play(0, 0, 0); // no looping: the buffer holds the whole song
}

void AudioOutput::stop()
{
    if (_buffer)
        static_cast<LPDIRECTSOUNDBUFFER>(_buffer)->Stop();
}

long AudioOutput::currentFrame() const
{
    if (!_buffer)
        return 0;
    DWORD playCursor = 0;
    static_cast<LPDIRECTSOUNDBUFFER>(_buffer)->GetCurrentPosition(&playCursor, nullptr);
    return static_cast<long>(playCursor / kBytesPerFrame);
}

void AudioOutput::close()
{
    if (_buffer) {
        auto* buffer = static_cast<LPDIRECTSOUNDBUFFER>(_buffer);
        buffer->Stop();
        buffer->Release();
        _buffer = nullptr;
    }
    if (_dsound) {
        static_cast<LPDIRECTSOUND8>(_dsound)->Release();
        _dsound = nullptr;
    }
}

} // namespace ojgl
