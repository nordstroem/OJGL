#include "../window.h"
#include "music.h"

unsigned char song_v2m[] = {
#include "song.inc"
};

Music::Music()
{
    player = std::make_unique<V2MPlayer>();
}

Music::~Music()
{
    player->Close();
}

void Music::play()
{

    player->Init();
    player->Open(&(song_v2m[0]));
    player->Play();
    while (player->IsPlaying())
        player->Tick();
    player->Stop();
    dsInit(player->RenderProxy, player.get(), GetForegroundWindow());
    player->Play();
}
