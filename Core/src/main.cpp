
#include "gl_state.h"
#include "music/v2mplayer.h"
#include "timer.h"
#include "window.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>
#include <thread>
#include <vector>

using namespace std;

int main()
{
    ifstream file("C:/Users/Oskar/Desktop/song.v2m", ios::binary);

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    cout << buffer.size() << "\n";
    V2MPlayer* player = new V2MPlayer();
    player->Init();
    cout << "Open: " << player->Open(&(buffer[0])) << "\n";
    player->Play();
    while (player->IsPlaying())
        player->Tick();
    player->Stop();
    dsInit(player->RenderProxy, player, GetForegroundWindow());
    player->Play();
    bool fullScreen = false;
    Window window(fullScreen);
    GLState glState;
    while (true) {
        //cout << Timer::time<Timer::ms_t>([&]() {
        window.getMessages();
        glState.render();
        //}) << '\n';
    }

    return 0;
}