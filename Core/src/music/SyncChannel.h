#pragma once
#include <queue>
#include <vector>

class SyncChannel {
public:
    SyncChannel();
    SyncChannel(int numNotes, int minNote, int channel);
    ~SyncChannel();
    void pushNote(int absoluteNote, int time);
    void tick(long long currentTime);
    float getTimeToNext(int absoluteNote) const;
    float getTimeSinceLast(int absoluteNote) const;
    int getTotalHitsPerNote(int absoluteNote) const;
    int getTotalHits() const;
    int numNotes;
    int channel;

private:
    int _minNote;
    long long _currentTime;
    std::vector<int> _lastTimePerNote;
    std::vector<std::queue<int>> _timesPerNote;
    std::vector<int> _totalHitsPerNote;
};
