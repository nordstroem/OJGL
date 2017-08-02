#pragma once

#include "v2mplayer.h"
#include <memory>
#include <vector>

extern unsigned char song_v2m[];

class Music {
public:
    Music();
    ~Music();
    void play();
    void sync(int channel, int note, int volume);

private:
    std::unique_ptr<V2MPlayer> player;
    bool playerInitialized = false;
};
