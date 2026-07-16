#include "Music.h"
#include "MusicPlayer.h"
#include "utility/Log.h"
#include "utility/OJstd.h"
#include <limits>

namespace ojgl {

Music::Music(const ojstd::shared_ptr<MusicPlayer>& player, bool fixedTimestep)
    : _player(player)
    , _fixedTimestep(fixedTimestep)
{
}

Music::~Music() = default;

static ojstd::shared_ptr<Music> music;

ojstd::shared_ptr<Music> Music::instance()
{
    return music;
}

void Music::createInstance(bool fixedTimestep)
{
    if (music != nullptr) {
        _ASSERT_EXPR(false, "Music already created");
    }
    auto player = createSelectedPlayer();
    if (player == nullptr)
        return; // no song embedded -> demo runs without music, singleton stays null
    music = ojstd::shared_ptr<Music>(new Music(player, fixedTimestep));
}

void Music::play()
{
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
    }
    return this->_player->elapsedTime();
}

void Music::stop()
{
    this->_player->stop();
}

ojstd::unordered_map<int, SyncChannel>& Music::syncChannels()
{
    return _syncChannels;
}

void Music::setTime(Duration time)
{
    // (Re)start/seek playback; the backend leaves fresh sync events for _initSync() to consume.
    this->_player->play(time);
    // SyncChannel's per-note queues only drain forward (tick() pops from the front), so without
    // rebuilding here, seeking backward (e.g. restart) leaves _lastTimePerNote at a later
    // timestamp than the now-earlier elapsedTime(), making getTimeSinceAnyNote() go negative.
    _syncChannels = ojstd::unordered_map<int, SyncChannel>();
    _initSync();
    // Drain the rebuilt channels up to the seek target before returning. GLState renders a frame
    // before it calls updateSync(), so leaving them at zero here would make every channel report
    // no hits at all for the first frame after any seek.
    for (auto& kv : _syncChannels) {
        kv.second.tick(time);
    }
}
} //namespace ojgl
