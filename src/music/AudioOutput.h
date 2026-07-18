#pragma once

namespace ojgl {

class AudioOutput {
public:
    AudioOutput() = default;
    AudioOutput(const AudioOutput&) = delete;
    AudioOutput& operator=(const AudioOutput&) = delete;
    ~AudioOutput();

    void init(const void* interleavedStereo, unsigned long numFrames, unsigned long sampleRate, unsigned short bitsPerSample, void* hWnd);

    void play(unsigned long startFrame);
    void stop();
    void close();

    long currentFrame() const;

private:
    void* _dsound = nullptr; // IDirectSound8*
    void* _buffer = nullptr; // IDirectSoundBuffer*
    unsigned long _numFrames = 0;
    unsigned long _bytesPerFrame = 0; // 2 channels * bitsPerSample/8
};

} // namespace ojgl
