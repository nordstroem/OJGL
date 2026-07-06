#include "ClinksterPlayer.h"
#include "thirdparty/clinkster/clinkster.h"
#include "utility/OJstd.h"
#include <windows.h>

namespace ojgl {

// Clinkster always renders 16-bit stereo PCM at 44100 Hz (struct sample { short left, right; }).
static constexpr unsigned long kSampleRate = 44100;

ClinksterPlayer::ClinksterPlayer()
    : _audio(ojstd::make_shared<AudioOutput>())
{
}

ClinksterPlayer::~ClinksterPlayer()
{
    stopAudio();
    if (_renderThread) {
        WaitForSingleObject(static_cast<HANDLE>(_renderThread), INFINITE);
        CloseHandle(static_cast<HANDLE>(_renderThread));
        _renderThread = nullptr;
    }
}

unsigned long __stdcall ClinksterPlayer::renderThreadProc(void* self)
{
    auto* player = static_cast<ClinksterPlayer*>(self);
    // Fills the static Clinkster_MusicBuffer[]; takes a few seconds.
    Clinkster_GenerateMusic();
    InterlockedExchange(&player->_renderDone, 1);
    return 0;
}

void ClinksterPlayer::beginRender()
{
    if (_renderThread || _renderDone)
        return;
    _renderThread = CreateThread(nullptr, 0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(&ClinksterPlayer::renderThreadProc), this, 0, nullptr);
}

bool ClinksterPlayer::renderDone() const
{
    return _renderDone != 0;
}

void ClinksterPlayer::startAudio(unsigned long startMs, void* hWnd)
{
    // Clinkster_WavFileHeader[10] is the WAV "data" chunk size in bytes; 4 bytes per stereo frame.
    const unsigned long numFrames = Clinkster_WavFileHeader[10] / 4;

    // tlibc lacks the __ftol2_sse / 64-bit division helpers MSVC emits for direct float->int and
    // long division, so do the arithmetic in float and convert with ojstd::ftoi.
    long startFrame = ojstd::ftoi(startMs / 1000.0f * static_cast<float>(kSampleRate));
    if (startFrame < 0)
        startFrame = 0;
    if (startFrame > static_cast<long>(numFrames))
        startFrame = static_cast<long>(numFrames);

    _audio->init(Clinkster_MusicBuffer, numFrames, kSampleRate, 16, hWnd);
    _audio->play(static_cast<unsigned long>(startFrame));
}

void ClinksterPlayer::stopAudio()
{
    _audio->close();
}

long ClinksterPlayer::elapsedMilliseconds() const
{
    return ojstd::ftoi(_audio->currentFrame() / static_cast<float>(kSampleRate) * 1000.0f);
}

} // namespace ojgl
