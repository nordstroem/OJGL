#pragma once
#include "utility\Timer.hpp"
#include <queue>
#include <vector>

namespace ojgl {

class SyncChannel {
public:
    SyncChannel() = default;
    SyncChannel(int numNotes, int minNote, int channel);
    void pushNote(int absoluteNote, Milliseconds time);
    void tick(Milliseconds currentTime);
    Milliseconds getTimeToNext(int relativeNote) const;
    Milliseconds getTimeSinceLast(int relativeNote) const;
    int getTotalHitsPerNote(int relativeNote) const;
    int getTotalHits() const;
    int numNotes{ -1 };
    int channel{ -1 };

private:
    int _minNote{ -1 };
    Milliseconds _currentTime;
    std::vector<Milliseconds> _lastTimePerNote;
    std::vector<std::queue<Milliseconds>> _timesPerNote;
    std::vector<int> _totalHitsPerNote;
};
} //namespace ojgl
