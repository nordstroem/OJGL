#pragma once

#include "syncchannel.h"
#include "v2mplayer.h"
#include <map>
#include <memory>

namespace ojgl {

class Music {

public:
    Music(unsigned char* song);
    ~Music();
    void play();
    void updateSync();

private:
    void initSync();
    std::map<int, SyncChannel> _syncChannels;
    unsigned char* _song;
    std::unique_ptr<V2MPlayer> _player;
};
} //namespace ojgl