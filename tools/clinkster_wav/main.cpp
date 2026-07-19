// Standalone renderer: turns a Clinkster song (assembled from clinkster.asm + a converted
// song.asm) into a normal .wav file, without needing Renoise's own audio export at all.
// Clinkster always synthesizes the whole song into Clinkster_MusicBuffer up front; the engine
// even ships a ready-made 44-byte WAV header for exactly this purpose (see clinkster.h).
#include "clinkster.h"
#include <cstdio>

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::fprintf(stderr, "Usage: %s <output.wav>\n", argv[0]);
        return 1;
    }

    std::printf("Rendering...\n");
    Clinkster_GenerateMusic();

    const char* outPath = argv[1];
    FILE* f = std::fopen(outPath, "wb");
    if (!f) {
        std::fprintf(stderr, "Failed to open '%s' for writing\n", outPath);
        return 1;
    }

    std::fwrite(Clinkster_WavFileHeader, 1, 44, f);
    std::fwrite(Clinkster_MusicBuffer, 1, Clinkster_WavFileHeader[10], f);
    std::fclose(f);

    std::printf("Wrote %s (%u bytes of audio, %.1f seconds)\n", outPath, Clinkster_WavFileHeader[10],
        Clinkster_WavFileHeader[10] / 4.0 / 44100.0);
    return 0;
}
