#pragma once
#include <vector>

class SyncChannel {
public:
    SyncChannel();
    SyncChannel(int numNotes, int minNote, int channel, double decay = 1.0);
    ~SyncChannel();
    void tick();
    void setVelocity(int absoluteNote, double velocity);
    double getNoteVelocity(int relativeNote);
    double getTotalNoteVelocity();
    int getTotalHitsPerNote(int absoluteNote);
    int getTotalHits();
    int numNotes;
    int channel;

private:
    double _decay;
    int _minNote;
    std::vector<double> _currentNoteVelocities;
    std::vector<int> _totalHitsPerNote;
};
