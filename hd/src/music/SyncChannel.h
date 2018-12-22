#pragma once
#include "..\..\fakelibs\fakelib.h"
#include "..\utility\Timer.hpp"

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
    fl::vector<Duration> _lastTimePerNote;
    fl::vector<fl::vector<Duration>> _timesPerNote;
    fl::vector<int> _totalHitsPerNote;
};
} //namespace ojgl
