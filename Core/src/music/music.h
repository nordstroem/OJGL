#pragma once

#include "v2mplayer.h"
#include <memory>
#include <vector>

class Music {
public:
    Music();
    ~Music();
    void play();

private:
    std::vector<unsigned char> song_v2m;
    std::unique_ptr<V2MPlayer> player;
};
