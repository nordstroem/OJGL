#include "Music.h"
#include "V2MPlayer.h"
#include "utility/Log.h"
#include "utility/OJstd.h"
#include <limits>
#include <windows.h>

namespace ojgl {

Music::Music(const unsigned char* song, bool fixedTimestep)
    : _song(song)
    , _player(ojstd::make_shared<V2MPlayer>())
    , _fixedTimestep(fixedTimestep)
{
}

Music::~Music()
{
    this->_player->Close();
    dsClose();
}

static ojstd::shared_ptr<Music> music;

ojstd::shared_ptr<Music> Music::instance()
{
    return music;
}

void Music::createInstance(const unsigned char* song, bool fixedTimestep)
{
    if (music != nullptr) {
        _ASSERT_EXPR(false, "Music already created");
    }
    music = ojstd::shared_ptr<Music>(new Music(song, fixedTimestep));
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
    _currentFrame++;
}

Duration Music::elapsedTime() const
{
    if (_fixedTimestep) {
        return Duration(1000 * _currentFrame / 60);
    } else {
        // @todo verify this formula.
        long ms = ojstd::ftoi(dsGetCurSmp() * 1000.f / (44100.f * 4.f));
        return Duration::milliseconds(ms) + _syncOffset;
    }
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
