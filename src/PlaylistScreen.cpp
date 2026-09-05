#include "PlaylistScreen.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

PlaylistScreen::PlaylistScreen(UIRenderer& renderer, InputHandler& input,
                               Player& player, vector<Playlist*>& playlists)
    : Screen(renderer, input), player(player), playlists(playlists) {}

void PlaylistScreen::render() {
    renderer.clearScreen();
    renderer.drawTopBorder();
    renderer.drawRow("Playlists");
    renderer.drawSeparator();

    if (playlists.empty()) {
        renderer.drawRow("No playlists found.");
        renderer.drawSeparator();
        renderer.drawRow("0. Back");
        renderer.drawBottomBorder();
        return;
    }

    renderer.drawRow("#   Name                          Songs");
    renderer.drawSeparator();

    Playlist* active = player.getActivePlaylist();
    for (int i = 0; i < static_cast<int>(playlists.size()); ++i) {
        Playlist* pl = playlists[i];
        bool isActive = (pl == active);

        stringstream row;
        row << left << setw(3) << (to_string(i + 1) + ".") << " "
            << (isActive ? "\u25B6 " : "  ")
            << setw(26) << pl->getName()
            << setw(3) << pl->getSongCount();
        if (isActive) row << " [active]";
        renderer.drawRow(row.str());
    }

    renderer.drawSeparator();
    renderer.drawRow("Enter a number to switch the active playlist.");
    renderer.drawRow("0. Back");
    renderer.drawBottomBorder();
}

int PlaylistScreen::handleInput() {
    if (playlists.empty()) {
        input.readInt(0, 0, "Choice: ");
        return NAV_BACK;
    }

    int choice = input.readInt(0, static_cast<int>(playlists.size()), "Choice: ");
    if (choice == 0) return NAV_BACK;

    Playlist* selected = playlists[choice - 1];
    if (selected != player.getActivePlaylist()) {
        player.switchActivePlaylist(selected);
    }
    return NAV_STAY;
}
