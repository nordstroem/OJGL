#include "../window.h"
#include "music.h"
#include <iostream>

unsigned char song_v2m[] = {
#include "song.inc"
};

Music::Music()
{
    player = std::make_unique<V2MPlayer>(*this);
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
    playerInitialized = true;
    player->Play();
}

void Music::sync(int channel, int note, int velocity)
{
    if (playerInitialized) {
        std::cout << "Channel: " << channel << " Note: " << note << " Velocity: " << velocity << "\n";
    }
}
