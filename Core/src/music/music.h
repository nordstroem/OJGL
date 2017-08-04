#pragma once

#include "v2mplayer.h"
#include <memory>
#include <vector>

namespace ojgl {

class Music {

public:
    Music(unsigned char* song);
    ~Music();
    void play();
    void updateSync();

private:
    unsigned char* _song;
    std::unique_ptr<V2MPlayer> _player;
    bool _playerInitialized = false;
};
} //namespace ojgl