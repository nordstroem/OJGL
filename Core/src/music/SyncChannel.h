#pragma once
#include "utility\Timer.hpp"
#include <queue>
#include <vector>

namespace ojgl {

class SyncChannel {
public:
    SyncChannel() = default;
    SyncChannel(int numNotes, int minNote, int channel);
    void pushNote(int absoluteNote, Duration time);
    void tick(Duration currentTime);
    Duration getTimeToNext(int relativeNote) const;
    Duration getTimeSinceLast(int relativeNote) const;
    int getTotalHitsPerNote(int relativeNote) const;
    int getTotalHits() const;
    int numNotes{ -1 };
    int channel{ -1 };

private:
    int _minNote{ -1 };
    Duration _currentTime;
    std::vector<Duration> _lastTimePerNote;
    std::vector<std::queue<Duration>> _timesPerNote;
    std::vector<int> _totalHitsPerNote;
};
} //namespace ojgl
