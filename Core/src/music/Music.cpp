#include "Music.h"
#include "render/Window.h"
#include "utility/Log.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

namespace ojgl {

Music::Music(unsigned char* song)
    : _song(song)
    , _player(std::make_unique<V2MPlayer>())
    , _startTime(Timepoint::now())
{
}

Music::~Music()
{
    this->_player->Close();
}

void Music::play()
{
    this->_player->Init();
    this->_player->Open(this->_song);
    setTime(Duration::milliseconds(0));
}

void Music::_initSync()
{
    std::unordered_map<int, int> channelMinNote;
    std::unordered_map<int, int> channelMaxNote;
    std::unordered_set<int> channels;
    auto events = _player->popSyncEvents();
    for (auto& se : events) {
        int channel = se.channel;
        int note = se.note;

        channels.insert(channel);

        if (channelMinNote.find(channel) == channelMinNote.end()) {
            channelMinNote[channel] = (std::numeric_limits<int>::max)();
        }
        if (channelMaxNote.find(channel) == channelMaxNote.end()) {
            channelMaxNote[channel] = std::numeric_limits<int>::lowest();
        }

        channelMinNote[channel] = min(channelMinNote[channel], note);
        channelMaxNote[channel] = max(channelMaxNote[channel], note);
    }

    for (auto& c : channels) {
        int numNotes = channelMaxNote[c] - channelMinNote[c] + 1;
        LOG_INFO("Channel " << c << " with " << numNotes << " notes");
        syncChannels[c] = SyncChannel(numNotes, channelMinNote[c], c);
    }

    for (auto& se : events) {
        syncChannels[se.channel].pushNote(se.note, se.time);
    }
}

void Music::updateSync()
{
    auto time = Timepoint::now() - _startTime;
    for (auto& kv : syncChannels) {
        kv.second.tick(time);
    }
}

void Music::stop()
{
    this->_player->Stop();
}

void Music::setTime(Duration time)
{
    auto ms = static_cast<sU32>(time.toMilliseconds());
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
    _startTime = Timepoint::now() - time;
}
} //namespace ojgl
