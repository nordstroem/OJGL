#pragma once
#include "utility\Timer.hpp"
#include <queue>
#include <vector>

namespace ojgl {

class SyncChannel {
public:
    SyncChannel() = default;
    SyncChannel(int numNotes, int minNote, int channel);
    void pushNote(int absoluteNote, timer::Milliseconds time);
    void tick(timer::Milliseconds currentTime);
    timer::Milliseconds getTimeToNext(int relativeNote) const;
    timer::Milliseconds getTimeSinceLast(int relativeNote) const;
    int getTotalHitsPerNote(int relativeNote) const;
    int getTotalHits() const;
    int numNotes{ -1 };
    int channel{ -1 };

private:
    int _minNote{ -1 };
    timer::Milliseconds _currentTime;
    std::vector<timer::Milliseconds> _lastTimePerNote;
    std::vector<std::queue<timer::Milliseconds>> _timesPerNote;
    std::vector<int> _totalHitsPerNote;
};
} //namespace ojgl
