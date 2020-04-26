#include "Music.h"
#include "V2MPlayer.h"
#include "utility/Log.h"
#include "utility/OJstd.h"
#include <limits>
#include <windows.h>

namespace ojgl {

Music::Music(unsigned char* song)
    : _song(song)
    , _player(ojstd::make_shared<V2MPlayer>())
{
}

Music::~Music()
{
    this->_player->Close();
    dsClose();
}

void Music::play()
{
    this->_player->Init();
    this->_player->Open(this->_song);
    setTime(Duration::milliseconds(0));
}

void Music::_initSync()
{
    ojstd::unordered_map<int, int> channelMinNote;
    ojstd::unordered_map<int, int> channelMaxNote;
    ojstd::unordered_set<int> channels;
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

        channelMinNote[channel] = ojstd::min(channelMinNote[channel], note);
        channelMaxNote[channel] = ojstd::max(channelMaxNote[channel], note);
    }

    for (auto& c : channels) {
        int numNotes = channelMaxNote[c] - channelMinNote[c] + 1;
        LOG_INFO("Channel " << c << " with " << numNotes << " notes");
        this->_syncChannels[c] = SyncChannel(numNotes, channelMinNote[c], c);
    }

    for (auto& se : events) {
        _syncChannels[se.channel].pushNote(se.note, se.time);
    }
}

void Music::updateSync()
{
    auto time = this->elapsedTime();
    for (auto& kv : _syncChannels) {
        kv.second.tick(time);
    }
}

Duration Music::elapsedTime() const
{
    // @todo verify this formula.
    return Duration::milliseconds(dsGetCurSmp() * 1000ll / (44100 * 4)) + _syncOffset;
}

void Music::stop()
{
    this->_player->Stop();
}

ojstd::unordered_map<int, SyncChannel>& Music::syncChannels()
{
    return _syncChannels;
}

void Music::setTime(Duration time)
{
    auto ms = time.toMilliseconds<sU32>();
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
    _syncOffset = time;
}
} //namespace ojgl
