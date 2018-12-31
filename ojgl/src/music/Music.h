#pragma once

#include "SyncChannel.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

class V2MPlayer;

class Music {

public:
    explicit Music(unsigned char* song);
    Music(const Music& other) = delete;
    Music& operator=(const Music& other) = delete;
    ~Music();

    void play();
    void updateSync();
    void setTime(Duration time);
    void stop();
    Duration elapsedTime() const;
    ojstd::unordered_map<int, SyncChannel>& syncChannels();

private:
    ojstd::shared_ptr<V2MPlayer> _player;
    void _initSync();
    unsigned char* _song;
    Duration _syncOffset;
    ojstd::unordered_map<int, SyncChannel> _syncChannels;
};

} //namespace ojgl
