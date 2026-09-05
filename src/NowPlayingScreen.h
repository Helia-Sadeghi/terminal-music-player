#pragma once
#include "Screen.h"
#include "Player.h"

class NowPlayingScreen : public Screen {
private:
    Player& player;

    void draw();                       
    bool processKey(int key);         
    static std::string fmt(double s);  

public:
    NowPlayingScreen(UIRenderer& renderer, InputHandler& input, Player& player);

    void render() override;
    int  handleInput() override;
};
