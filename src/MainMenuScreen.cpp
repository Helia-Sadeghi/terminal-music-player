#include "MainMenuScreen.h"
#include <iostream>

using namespace std;

MainMenuScreen::MainMenuScreen(UIRenderer& renderer, InputHandler& input,
                               Player& player, ConfigManager& config,
                               MusicLibrary& library)
    : Screen(renderer, input), player(player), config(config), library(library) {}

void MainMenuScreen::render() {
    renderer.clearScreen();
    renderer.drawTopBorder();
    renderer.drawCenteredRow("\u266B Terminal Music Player \u266B");
    renderer.drawSeparator();

    Song* last = player.getLastPlayedSong();
    if (last == nullptr) {
        string path = config.get("last_song", "");
        if (!path.empty()) last = library.getSongByPath(path);
    }
    if (last != nullptr) {
        renderer.drawRow("Last played: " + last->getTitle() +
                         " \u2014 " + last->getArtist());
        renderer.drawSeparator();
    }

    renderer.drawRow("1. Now Playing");
    renderer.drawRow("2. Playlists");
    renderer.drawRow("3. Browse Playlist");
    renderer.drawRow("4. Settings");
    renderer.drawSeparator();
    renderer.drawRow("0. Quit (saves state)");
    renderer.drawBottomBorder();
}

int MainMenuScreen::handleInput() {
    int choice = input.readInt(0, 4, "Enter choice: ");
    switch (choice) {
        case 1: return NAV_OPEN_NOW;
        case 2: return NAV_OPEN_LISTS;
        case 3: return NAV_OPEN_BROWSE;
        case 4: return NAV_OPEN_SETTINGS;
        case 0: return NAV_QUIT;
    }
    return NAV_STAY;
}
