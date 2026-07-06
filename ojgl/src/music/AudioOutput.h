#pragma once

namespace ojgl {

// Minimal DirectSound output that plays a single, pre-rendered, interleaved-stereo float buffer
// (the whole song). Adapted from sointu's examples/code/C/cplay.windows.directsound.c.
//
// 4klang renders the entire song up front, so there is no streaming callback: we hand
// DirectSound the full song buffer once and read the play cursor for timing. This replaces the
// dsInit/dsGetCurSmp/dsClose helpers that libv2 provides for the V2 backend, so a 4klang build
// needs no dependency on libv2.
class AudioOutput {
public:
    AudioOutput() = default;
    AudioOutput(const AudioOutput&) = delete;
    AudioOutput& operator=(const AudioOutput&) = delete;
    ~AudioOutput();

    // Creates the secondary buffer and copies the whole song into it.
    // numFrames = number of stereo sample-frames (i.e. length of the float array / 2).
    void init(const float* interleavedStereo, unsigned long numFrames, unsigned long sampleRate, void* hWnd);

    // (Re)starts playback from the given stereo sample-frame offset.
    void play(unsigned long startFrame);
    void stop();
    void close();

    // Current play cursor in stereo sample-frames from the start of the song.
    long currentFrame() const;

private:
    void* _dsound = nullptr; // IDirectSound8*
    void* _buffer = nullptr; // IDirectSoundBuffer*
    unsigned long _numFrames = 0;
};

} // namespace ojgl
