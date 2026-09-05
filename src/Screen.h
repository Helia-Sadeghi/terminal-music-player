#pragma once
#include "UIRenderer.h"
#include "InputHandler.h"

enum NavResult {
    NAV_QUIT         = -1,  
    NAV_BACK         = 0,  
    NAV_STAY         = 1,   
    NAV_NOW_PLAYING  = 2,   
    NAV_OPEN_NOW     = 10,  
    NAV_OPEN_LISTS   = 11, 
    NAV_OPEN_BROWSE  = 12, 
    NAV_OPEN_SETTINGS= 13   
};

class Screen {
protected:
    UIRenderer&   renderer;
    InputHandler& input;

public:
    Screen(UIRenderer& renderer, InputHandler& input);
    virtual ~Screen() = default;

    virtual void render() = 0;     
    virtual int  handleInput() = 0;
};
