#include "Music.h"
#ifdef OJGL_SYNTH_4KLANG
#include "FourKlangPlayer.h"
#else
#include "V2MPlayer.h"
#endif
#include "utility/Log.h"
#include "utility/OJstd.h"
#include <limits>
#include <windows.h>

namespace ojgl {

Music::Music(const unsigned char* song, bool fixedTimestep)
    : _song(song)
#ifdef OJGL_SYNTH_4KLANG
    , _player(ojstd::make_shared<FourKlangPlayer>())
#else
    , _player(ojstd::make_shared<V2MPlayer>())
#endif
    , _fixedTimestep(fixedTimestep)
{
}

Music::~Music()
{
#ifdef OJGL_SYNTH_4KLANG
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
#ifdef OJGL_SYNTH_4KLANG
    // The song is baked into the assembled 4klang object; there is no runtime song pointer.
    // Kick off the (multi-second) render on a background thread; audio starts once it is done.
    this->_player->beginRender();
#else
    this->_player->Init();
    this->_player->Open(this->_song);
#endif
    setTime(Duration::milliseconds(0));
}

#ifdef OJGL_SYNTH_4KLANG
void Music::_tryStartAudio()
{
    if (!_audioStarted && _player->renderDone()) {
        _player->startAudio(_pendingStartMs, GetForegroundWindow());
        _audioStarted = true;
    }
}
#endif

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
#ifdef OJGL_SYNTH_4KLANG
    // Polled from the main loop: start playback as soon as the background render completes.
    _tryStartAudio();
#endif
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
#ifdef OJGL_SYNTH_4KLANG
        // Until the background render finishes and audio starts, sit at the requested offset.
        if (!_audioStarted)
            return _syncOffset;
        // DirectSound's play cursor is absolute song position, so no _syncOffset is added.
        return Duration::milliseconds(this->_player->elapsedMilliseconds());
#else
        // @todo verify this formula.
        long ms = ojstd::ftoi(dsGetCurSmp() * 1000.f / (44100.f * 4.f));
        return Duration::milliseconds(ms) + _syncOffset;
#endif
    }
}

void Music::stop()
{
#ifdef OJGL_SYNTH_4KLANG
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
#ifdef OJGL_SYNTH_4KLANG
    // 4klang exposes no note stream, so there is nothing to pre-tick; _initSync() simply builds
    // no channels (audio-only for now). Audio (re)start is deferred to _tryStartAudio() so we
    // never block on the background render; it fires here immediately if the render is done.
    _initSync();
    this->_player->stopAudio();
    _pendingStartMs = time.toMilliseconds<unsigned int>();
    _audioStarted = false;
    _syncOffset = time;
    _tryStartAudio();
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
