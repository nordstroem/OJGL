#include "MusicPlayer.h"

namespace ojgl {

// SYNTH None: the demo links no synth library at all. There is no player to build, so
// createSelectedPlayer() returns nullptr, Music is never instantiated, and the demo runs on the
// system clock (see GLState). This is the one translation unit compiled for the None backend.
ojstd::shared_ptr<MusicPlayer> createSelectedPlayer()
{
    return nullptr;
}

}
