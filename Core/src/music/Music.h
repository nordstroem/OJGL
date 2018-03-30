#pragma once

#include "SyncChannel.h"
#include "V2MPlayer.h"
#include "utility\Timer.hpp"
#include <unordered_map>

namespace ojgl {

class Music {

public:
    explicit Music(unsigned char* song);
    ~Music();
    void play();
    void updateSync();
    std::unordered_map<int, SyncChannel> syncChannels;
    template <typename T>
    void setTime(Duration<T> time);
    void stop();

private:
    void _initSync();
    unsigned char* _song;
    std::unique_ptr<V2MPlayer> _player;
    Timepoint _startTime;
};

template <typename T>
inline void Music::setTime(Duration<T> time)
{
    auto ms = static_cast<sU32>(duration_cast<Milliseconds>(time).count());
    this->_player->Stop();
    dsClose();
    auto events = _player->popSyncEvents();
    this->_player->Play(ms);
    while (this->_player->IsPlaying()) {
        this->_player->Tick();
    }
    this->_player->Stop();
    _initSync();
    dsInit(this->_player->RenderProxy, this->_player.get(), GetForegroundWindow());
    this->_player->Play(ms);
    _startTime = now() - time;
}
} //namespace ojgl
