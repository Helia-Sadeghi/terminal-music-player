#pragma once
#include <memory>
#include <vector>
#include <string>

#include "MusicLibrary.h"
#include "Player.h"
#include "Playlist.h"
#include "ConfigManager.h"
#include "UIRenderer.h"
#include "InputHandler.h"
#include "Screen.h"

class Application {
private:
    enum class ScreenType {
        MAIN_MENU,
        NOW_PLAYING,
        PLAYLIST_LIST,
        BROWSE_PLAYLIST,
        SETTINGS
    };

    MusicLibrary           library;
    std::vector<Playlist*> playlists;   
    Player                 player;
    ConfigManager          config;
    UIRenderer             renderer;
    InputHandler           input;

    std::unique_ptr<Screen> currentScreen;
    ScreenType              currentType;
    bool                    running;

    void createScreen(ScreenType type);
    void handleResult(int result);
    void applySavedSettings();
    void saveState();
    void cleanupPlaylists();
    Playlist* findPlaylistByName(const std::string& name) const;

public:
    Application();
    ~Application();

    bool init();
    void run();
};
