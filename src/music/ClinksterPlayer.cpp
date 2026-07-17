#include "ClinksterPlayer.h"
#include "thirdparty/clinkster/clinkster.h"
#include "utility/Macros.h"
#include "utility/OJstd.h"
#include <windows.h>

namespace ojgl {

#ifdef _DEBUG
// Clinkster always renders 16-bit stereo PCM at 44100 Hz (struct sample { short left, right; }).
static constexpr unsigned long kSampleRate = 44100;
#endif

ClinksterPlayer::ClinksterPlayer()
#ifdef _DEBUG
    : _audio(ojstd::make_shared<AudioOutput>())
#endif
{
}

ClinksterPlayer::~ClinksterPlayer()
{
#ifdef _DEBUG
    stopAudio();
#endif
}

ojstd::shared_ptr<MusicPlayer> createSelectedPlayer()
{
    return ojstd::make_shared<ClinksterPlayer>();
}

void ClinksterPlayer::play(Duration startTime)
{
    Clinkster_GenerateMusic();

#ifdef _DEBUG
    // startAudio re-seeks by repositioning the DirectSound play cursor (SetCurrentPosition), so
    // the cursor is absolute and elapsedMilliseconds() needs no offset.
    startAudio(startTime.toMilliseconds<unsigned long>(), GetForegroundWindow());
#else
    // Native Clinkster waveOut player: start-once and always from the beginning (it cannot seek).
    // Release never seeks -- scrubbing/restart are _DEBUG-only -- so startTime is always 0 here.
    OJ_UNUSED(startTime);
    if (!_started) {
        Clinkster_StartMusic();
        _started = true;
    }
#endif
}

void ClinksterPlayer::stop()
{
#ifdef _DEBUG
    stopAudio();
#endif
    // Release: Clinkster's native player exports no stop, and release never pauses (pause is
    // _DEBUG-only); the waveOut device is reclaimed on process exit.
}

Duration ClinksterPlayer::elapsedTime() const
{
#ifdef _DEBUG
    return Duration::milliseconds(elapsedMilliseconds());
#else
    if (!_started)
        return Duration::milliseconds(0);
    // Clinkster_GetPosition() returns the play position in ticks, already advanced by
    // CLINKSTER_TIMER_OFFSET to compensate for display latency (the reference engine's tuning).
    const float ms = Clinkster_GetPosition() / Clinkster_TicksPerSecond * 1000.0f;
    return Duration::milliseconds(ojstd::ftoi(ms));
#endif
}

#ifdef _DEBUG
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
    return ojstd::ftoi((_audio->currentFrame()) / static_cast<float>(kSampleRate) * 1000.0f);
}
#endif // _DEBUG

ojstd::vector<SyncEvent> ClinksterPlayer::popSyncEvents()
{
    ojstd::vector<SyncEvent> events;

    // Clinkster has no note pitch/velocity per track; every trigger on every track is
    // synthesized as note 0 so each channel ends up with exactly one relative note
    // (Music::_initSync computes numNotes = maxNote - minNote + 1 == 1 per channel).
    constexpr int kNote = 0;
    constexpr int kVelocity = 127;

    const unsigned int numTracks = Clinkster_NumTracks;
    const unsigned int musicLength = Clinkster_MusicLength; // ticks with real song data
    const unsigned int stride = Clinkster_NumTicks; // per-track block size in Clinkster_NoteTiming

    for (unsigned int track = 0; track < numTracks; track++) {
        int lastValue = -1; // sentinel: never equals a valid tick index
        for (unsigned int tick = 0; tick < musicLength; tick++) {
            int stored = static_cast<int>(Clinkster_NoteTiming[track * stride + tick]);
            // Rising edge: the table now records *this* tick as the latest trigger. Never trust
            // tick 0 as a genuine trigger -- BSS-zero-initialized entries before a track's first
            // real note alias with a legitimate "note at tick 0", and the two can't be told apart
            // from this table alone; skipping tick 0 is the safer failure mode (a late first hit
            // instead of a spurious one at song start).
            if (tick > 0 && stored == static_cast<int>(tick) && stored != lastValue) {
                float seconds = static_cast<float>(tick) / Clinkster_TicksPerSecond;
                long ms = ojstd::ftoi(seconds * 1000.0f);
                events.push_back(SyncEvent(static_cast<int>(track), kNote, kVelocity, Duration::milliseconds(ms)));
            }
            lastValue = stored;
        }
    }

    return events;
}

} // namespace ojgl
