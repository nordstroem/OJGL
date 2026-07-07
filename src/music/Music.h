#pragma once

#include "SyncChannel.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

class MusicPlayer;

class Music {

public:
    Music(const Music& other) = delete;
    Music& operator=(const Music& other) = delete;
    ~Music();

    static ojstd::shared_ptr<Music> instance();
    // Builds the singleton for the selected backend via createSelectedPlayer(). If no song is
    // embedded (createSelectedPlayer() returns nullptr) the singleton is left null and the demo
    // runs without music.
    static void createInstance(bool fixedTimestep);

    void play();
    void updateSync();
    void setTime(Duration time);
    void stop();
    Duration elapsedTime() const;
    ojstd::unordered_map<int, SyncChannel>& syncChannels();

private:
    Music(const ojstd::shared_ptr<MusicPlayer>& player, bool fixedTimestep);

private:
    ojstd::shared_ptr<MusicPlayer> _player;
    void _initSync();
    ojstd::unordered_map<int, SyncChannel> _syncChannels;
    int _currentFrame = 0;
    bool _fixedTimestep = false;
};

} //namespace ojgl
