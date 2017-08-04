#include "music.h"
#include "render/Window.h"
#include <iostream>
#include <vector>

namespace ojgl {

Music::Music(unsigned char* song)
{
    this->_player = std::make_unique<V2MPlayer>();
    this->_song = song;
}

Music::~Music()
{
    this->_player->Close();
}

void Music::play()
{
    this->_player->Init();
    this->_player->Open(this->_song);
    this->_player->Play();
    while (this->_player->IsPlaying())
        this->_player->Tick();
    this->_player->Stop();
    this->_player->popSyncEvents();
    dsInit(this->_player->RenderProxy, this->_player.get(), GetForegroundWindow());
    this->_playerInitialized = true;
    this->_player->Play();
}

void Music::updateSync()
{
    for (auto& se : this->_player->popSyncEvents()) {
        std::cout << "Channel: " << se.channel << " Note: " << se.note << " Velocity: " << se.velocity << "\n";
    }
    std::cout << "\n";
}
} //namespace ojgl