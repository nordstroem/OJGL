#include "SyncChannel.h"
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace ojgl {

SyncChannel::SyncChannel()
    : _minNote(-1)
    , channel(-1)
    , numNotes(-1)
{
}

SyncChannel::SyncChannel(int numNotes, int minNote, int channel)
    : _minNote(minNote)
    , channel(channel)
    , numNotes(numNotes)
{
    _lastTimePerNote.resize(numNotes);
    _timesPerNote.resize(numNotes);
    _totalHitsPerNote.resize(numNotes);
}

SyncChannel::~SyncChannel()
{
}

void SyncChannel::pushNote(int absoluteNote, timer::ms_t time)
{
    _timesPerNote[absoluteNote - _minNote].push(time);
}

void SyncChannel::tick(timer::ms_t currentTime)
{
    _currentTime = currentTime;
    for (int note = 0; note < numNotes; note++) {
        std::queue<timer::ms_t>& s = _timesPerNote[note];
        while (!s.empty() && s.front() <= _currentTime) {
            _lastTimePerNote[note] = _currentTime;
            _totalHitsPerNote[note]++;
            s.pop();
        }
    }
}

timer::ms_t SyncChannel::getTimeToNext(int relativeNote) const
{
    const std::queue<timer::ms_t>& times = _timesPerNote[relativeNote];
    if (times.empty())
        return timer::ms_t(std::numeric_limits<long long>::max());
    return times.front() - _currentTime;
}

timer::ms_t SyncChannel::getTimeSinceLast(int relativeNote) const
{
    if (_totalHitsPerNote[relativeNote] == 0)
        return timer::ms_t(std::numeric_limits<long long>::max());
    return _currentTime - _lastTimePerNote[relativeNote];
}

int SyncChannel::getTotalHitsPerNote(int relativeNote) const
{
    return _totalHitsPerNote[relativeNote];
}

int SyncChannel::getTotalHits() const
{
    return std::accumulate(_totalHitsPerNote.begin(), _totalHitsPerNote.end(), 0);
}
} //namespace ojgl
