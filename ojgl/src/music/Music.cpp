#include "Music.h"
#ifdef OJGL_SYNTH_CLINKSTER
#include "ClinksterPlayer.h"
#else
#include "V2MPlayer.h"
#endif
#include "utility/Log.h"
#include "utility/OJstd.h"
#include <limits>
#include <windows.h>

namespace ojgl {

#ifdef OJGL_SYNTH_CLINKSTER
Music::Music(const unsigned char* song, bool fixedTimestep)
    : _song(song)
    , _player(ojstd::make_shared<ClinksterPlayer>())
    , _fixedTimestep(fixedTimestep)
{
}
#else
Music::Music(const unsigned char* song, bool fixedTimestep)
    : _song(song)
    , _player(ojstd::make_shared<V2MPlayer>())
    , _fixedTimestep(fixedTimestep)
{
}
#endif

Music::~Music()
{
#ifdef OJGL_SYNTH_CLINKSTER
    this->_player->stopAudio();
#else
    this->_player->Close();
    dsClose();
#endif
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
#ifdef OJGL_SYNTH_CLINKSTER
    // Render the whole song up front (a few seconds); a startup busy-wait is fine for a first
    // pass (see the deferred "loading state" follow-up), then start audio from the beginning.
    this->_player->beginRender();
    while (!this->_player->renderDone())
        Sleep(10);
    _initSync(); // populate _syncChannels now that Clinkster_NoteTiming is filled
    this->_player->startAudio(0, GetForegroundWindow());
#else
    this->_player->Init();
    this->_player->Open(this->_song);
    setTime(Duration::milliseconds(0));
#endif
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
#ifdef OJGL_SYNTH_CLINKSTER
        // The DirectSound play cursor already reflects the seek position (SetCurrentPosition),
        // so elapsedMilliseconds() is absolute; _syncOffset stays 0 for this backend.
        return Duration::milliseconds(this->_player->elapsedMilliseconds()) + _syncOffset;
#else
        // @todo verify this formula.
        long ms = ojstd::ftoi(dsGetCurSmp() * 1000.f / (44100.f * 4.f));
        return Duration::milliseconds(ms) + _syncOffset;
#endif
    }
}

void Music::stop()
{
#ifdef OJGL_SYNTH_CLINKSTER
    this->_player->stopAudio();
#else
    this->_player->Stop();
#endif
}

ojstd::unordered_map<int, SyncChannel>& Music::syncChannels()
{
    return _syncChannels;
}

void Music::setTime(Duration time)
{
#ifdef OJGL_SYNTH_CLINKSTER
    // Re-seek by repositioning the DirectSound play cursor; no libv2 tick loop. _syncOffset is
    // left at 0 because startAudio positions the cursor at the absolute song offset.
    this->_player->startAudio(time.toMilliseconds<unsigned long>(), GetForegroundWindow());
#else
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
#endif
}
} //namespace ojgl
