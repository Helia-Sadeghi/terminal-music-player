#pragma once
#include "Screen.h"
#include "Player.h"
#include "ConfigManager.h"
#include "MusicLibrary.h"

class MainMenuScreen : public Screen {
private:
    Player&        player;
    ConfigManager& config;
    MusicLibrary&  library;

public:
    MainMenuScreen(UIRenderer& renderer, InputHandler& input,
                   Player& player, ConfigManager& config, MusicLibrary& library);

    void render() override;
    int  handleInput() override;
};
