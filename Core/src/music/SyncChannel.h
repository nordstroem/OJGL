#pragma once
#include "utility\Timer.hpp"
#include <queue>
#include <vector>

namespace ojgl {

class SyncChannel {
public:
    SyncChannel();
    SyncChannel(int numNotes, int minNote, int channel);
    ~SyncChannel();
    void pushNote(int absoluteNote, timer::ms_t time);
    void tick(timer::ms_t currentTime);
    timer::ms_t getTimeToNext(int relativeNote) const;
    timer::ms_t getTimeSinceLast(int relativeNote) const;
    int getTotalHitsPerNote(int relativeNote) const;
    int getTotalHits() const;
    int numNotes{ -1 };
    int channel{ -1 };

private:
    int _minNote{ -1 };
    timer::ms_t _currentTime;
    std::vector<timer::ms_t> _lastTimePerNote;
    std::vector<std::queue<timer::ms_t>> _timesPerNote;
    std::vector<int> _totalHitsPerNote;
};
} //namespace ojgl
