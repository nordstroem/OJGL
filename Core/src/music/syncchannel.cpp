#include "syncchannel.h"
#include <stdexcept>

SyncChannel::SyncChannel()
    : _decay(-1.0)
    , _minNote(-1)
    , channel(-1)
    , numNotes(-1)
{
    //throw std::logic_error("This constructor should not be called");
}

SyncChannel::SyncChannel(int numNotes, int minNote, int channel, double decay)
    : _decay(decay)
    , _minNote(minNote)
    , channel(channel)
    , numNotes(numNotes)
{
    _currentNoteVelocities.resize(numNotes);
    _totalHitsPerNote.resize(numNotes);
}

SyncChannel::~SyncChannel()
{
}

void SyncChannel::tick()
{
    for (auto& velocity : _currentNoteVelocities)
        velocity *= 0.95f;
}

void SyncChannel::setVelocity(int absoluteNote, double velocity)
{
    if (velocity > 1.0) {
        _totalHitsPerNote[absoluteNote - _minNote]++;
        _currentNoteVelocities[absoluteNote - _minNote] = velocity;
    }
}

double SyncChannel::getNoteVelocity(int relativeNote)
{
    return _currentNoteVelocities[relativeNote];
}

double SyncChannel::getTotalNoteVelocity()
{
    double sum = 0.0;
    for (auto& velocity : _currentNoteVelocities)
        sum += velocity;
    return sum;
}

int SyncChannel::getTotalHitsPerNote(int relativeNote)
{
    return _totalHitsPerNote[relativeNote];
}

int SyncChannel::getTotalHits()
{
    int sum = 0;
    for (auto& amount : _totalHitsPerNote)
        sum += amount;
    return sum;
}
