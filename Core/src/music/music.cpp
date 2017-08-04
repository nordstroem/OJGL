#include "music.h"
#include "render/Window.h"
#include <iostream>
#include <map>
#include <set>

namespace ojgl {

Music::Music(unsigned char* song)
{
    this->_player = std::make_unique<V2MPlayer>();
    this->_song = song;
}

Music::~Music()
{
    this->_player->Close();
}

void Music::play()
{
    this->_player->Init();
    this->_player->Open(this->_song);
    this->_player->Play();
    while (this->_player->IsPlaying())
        this->_player->Tick();
    this->_player->Stop();
    initSync();
    dsInit(this->_player->RenderProxy, this->_player.get(), GetForegroundWindow());
    this->_player->Play();
}

void Music::initSync()
{
    const int MIN = -1;
    const int MAX = 999999;
    std::map<int, int> channelMinNote;
    std::map<int, int> channelMaxNote;
    std::set<int> channels;
    for (auto& se : _player->popSyncEvents()) {
        int channel = se.channel;
        int note = se.note;

        channels.insert(channel);

        if (channelMinNote.find(channel) == channelMinNote.end())
            channelMinNote[channel] = MAX;
        if (channelMaxNote.find(channel) == channelMaxNote.end())
            channelMaxNote[channel] = MIN;

        channelMinNote[channel] = min(channelMinNote[channel], note);
        channelMaxNote[channel] = max(channelMaxNote[channel], note);
    }

    for (auto& c : channels) {
        _syncChannels[c] = SyncChannel(channelMaxNote[c] - channelMinNote[c] + 1, channelMinNote[c]);
    }
}

void Music::updateSync()
{
    for (auto& se : this->_player->popSyncEvents()) {
        _syncChannels[se.channel].setVelocity(se.note, (double)se.velocity);
    }

    for (auto& kv : _syncChannels) {
        kv.second.tick();
    }
}

} //namespace ojgl