#include "SettingsScreen.h"
#include <iostream>

using namespace std;

SettingsScreen::SettingsScreen(UIRenderer& renderer, InputHandler& input,
                               Player& player, ConfigManager& config)
    : Screen(renderer, input), player(player), config(config) {}

void SettingsScreen::render() {
    renderer.clearScreen();
    renderer.drawTopBorder();
    renderer.drawRow("\u2699 Settings");
    renderer.drawSeparator();
    renderer.drawRow("Playback Mode (current: " + player.getModeString() + ")");
    renderer.drawSeparator();

    struct Item { const char* name; const char* desc; };
    const Item items[4] = {
        {"NO_REPEAT",  "play in order, stop at end"},
        {"REPEAT_ONE", "repeat current song forever"},
        {"REPEAT_ALL", "loop whole playlist"},
        {"SHUFFLE",    "random order"}
    };

    string cur = player.getModeString();
    for (int i = 0; i < 4; ++i) {
        string line = to_string(i + 1) + ". " + items[i].name;
        while (line.size() < 16) line += " ";
        line += items[i].desc;
        if (cur == items[i].name) line += "  \u25C0 (active)";
        renderer.drawRow(line);
    }

    renderer.drawSeparator();
    renderer.drawRow("0. Back (changes saved automatically)");
    renderer.drawBottomBorder();
}

int SettingsScreen::handleInput() {
    int choice = input.readInt(0, 4, "Choice: ");
    if (choice == 0) return NAV_BACK;

    switch (choice) {
        case 1: player.setMode(PlaybackMode::NO_REPEAT);  break;
        case 2: player.setMode(PlaybackMode::REPEAT_ONE); break;
        case 3: player.setMode(PlaybackMode::REPEAT_ALL); break;
        case 4: player.setMode(PlaybackMode::SHUFFLE);    break;
    }
    config.set("playback_mode", player.getModeString());
    config.save();
    return NAV_STAY;
}
