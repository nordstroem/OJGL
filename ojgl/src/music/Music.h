#pragma once

#include "SyncChannel.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

class V2MPlayer;

class Music {

public:
    Music(const Music& other) = delete;
    Music& operator=(const Music& other) = delete;
    ~Music();

    static ojstd::shared_ptr<Music> instance();
    static void createInstance(const unsigned char* song);

    void play();
    void updateSync();
    void setTime(Duration time);
    void stop();
    Duration elapsedTime() const;
    ojstd::unordered_map<int, SyncChannel>& syncChannels();

private:
    explicit Music(const unsigned char* song);

private:
    ojstd::shared_ptr<V2MPlayer> _player;
    void _initSync();
    const unsigned char* _song;
    Duration _syncOffset;
    ojstd::unordered_map<int, SyncChannel> _syncChannels;
    int _currentFrame = 0;
};

} //namespace ojgl
