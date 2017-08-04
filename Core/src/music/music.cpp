#include "../window.h"
#include "music.h"
#include <iostream>

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
    player->popSyncEvents();
    dsInit(player->RenderProxy, player.get(), GetForegroundWindow());
    playerInitialized = true;
    player->Play();
}

void Music::updateSync()
{
    for (auto& se : player->popSyncEvents()) {
        std::cout << "Channel: " << se.channel << " Note: " << se.note << " Velocity: " << se.velocity << "\n";
        //if (se.channel == 13 && se.velocity > 0)
        //    std::cout << "snare" << std::endl;
    }
    std::cout << "\n";
}
