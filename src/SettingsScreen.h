#pragma once
#include "Screen.h"
#include "Player.h"
#include "ConfigManager.h"

class SettingsScreen : public Screen {
private:
    Player&        player;
    ConfigManager& config;

public:
    SettingsScreen(UIRenderer& renderer, InputHandler& input,
                   Player& player, ConfigManager& config);

    void render() override;
    int  handleInput() override;
};
