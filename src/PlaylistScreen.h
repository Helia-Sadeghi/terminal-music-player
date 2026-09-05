#pragma once
#include "Screen.h"
#include "Player.h"
#include "Playlist.h"
#include <vector>

class PlaylistScreen : public Screen {
private:
    Player&                      player;
    std::vector<Playlist*>&      playlists;

public:
    PlaylistScreen(UIRenderer& renderer, InputHandler& input,
                   Player& player, std::vector<Playlist*>& playlists);

    void render() override;
    int  handleInput() override;
};
