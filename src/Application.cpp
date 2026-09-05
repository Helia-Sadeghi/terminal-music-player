#include "Application.h"
#include <iostream>

#include "CsvLoader.h"
#include "M3uLoader.h"
#include "MainMenuScreen.h"
#include "NowPlayingScreen.h"
#include "PlaylistScreen.h"
#include "BrowsePlaylistScreen.h"
#include "SettingsScreen.h"

using namespace std;

static const string CSV_PATH       = "Data/library.csv";
static const string PLAYLISTS_DIR  = "Data/Playlists";
static const string SETTINGS_PATH  = "Data/settings.cfg";
static const string MUSIC_BASE_DIR = "Data/";

Application::Application()
    : config(SETTINGS_PATH),
      currentScreen(nullptr),
      currentType(ScreenType::MAIN_MENU),
      running(true) {}

Application::~Application() {
    cleanupPlaylists();
}

bool Application::init() {
    player.setAudioBaseDir(MUSIC_BASE_DIR);

    config.load();  

    if (!CsvLoader::load(CSV_PATH, library)) {
        cerr << "Fatal: could not load " << CSV_PATH << "\n";
        return false;
    }

    M3uLoader::loadAll(PLAYLISTS_DIR, library, playlists);

    applySavedSettings();
    createScreen(ScreenType::MAIN_MENU);
    return true;
}

void Application::run() {
    while (running) {
        currentScreen->render();
        int result = currentScreen->handleInput();
        handleResult(result);
    }
    saveState();
}

Playlist* Application::findPlaylistByName(const string& name) const {
    for (Playlist* pl : playlists) {
        if (pl->getName() == name) return pl;
    }
    return nullptr;
}

void Application::createScreen(ScreenType type) {
    currentType = type;
    switch (type) {
        case ScreenType::MAIN_MENU:
            currentScreen = make_unique<MainMenuScreen>(
                renderer, input, player, config, library);
            break;
        case ScreenType::NOW_PLAYING:
            currentScreen = make_unique<NowPlayingScreen>(
                renderer, input, player);
            break;
        case ScreenType::PLAYLIST_LIST:
            currentScreen = make_unique<PlaylistScreen>(
                renderer, input, player, playlists);
            break;
        case ScreenType::BROWSE_PLAYLIST:
            currentScreen = make_unique<BrowsePlaylistScreen>(
                renderer, input, player);
            break;
        case ScreenType::SETTINGS:
            currentScreen = make_unique<SettingsScreen>(
                renderer, input, player, config);
            break;
    }
}

void Application::handleResult(int result) {
    if (result == NAV_STAY) return;

    switch (result) {
        case NAV_QUIT:
            running = false;
            return;
        case NAV_BACK:
            createScreen(ScreenType::MAIN_MENU);
            return;
        case NAV_NOW_PLAYING:
            createScreen(ScreenType::NOW_PLAYING);
            return;
        case NAV_OPEN_NOW:
            createScreen(ScreenType::NOW_PLAYING);
            return;
        case NAV_OPEN_LISTS:
            createScreen(ScreenType::PLAYLIST_LIST);
            return;
        case NAV_OPEN_BROWSE:
            createScreen(ScreenType::BROWSE_PLAYLIST);
            return;
        case NAV_OPEN_SETTINGS:
            createScreen(ScreenType::SETTINGS);
            return;
        default:
            return;   
    }
}

void Application::applySavedSettings() {
    // Playback mode
    string mode = config.get("playback_mode", "NO_REPEAT");
    if      (mode == "REPEAT_ONE") player.setMode(PlaybackMode::REPEAT_ONE);
    else if (mode == "REPEAT_ALL") player.setMode(PlaybackMode::REPEAT_ALL);
    else if (mode == "SHUFFLE")    player.setMode(PlaybackMode::SHUFFLE);
    else                           player.setMode(PlaybackMode::NO_REPEAT);


    Playlist* active = findPlaylistByName(config.get("active_playlist", ""));
    if (active == nullptr && !playlists.empty()) active = playlists.front();
    player.setActivePlaylist(active);

  
    string lastPath = config.get("last_song", "");
    if (!lastPath.empty()) {
        Playlist* host = nullptr;
        int idx = -1;
        if (active != nullptr) {            
            for (int i = 0; i < active->getSongCount(); ++i) {
                if (active->getSong(i)->getFilePath() == lastPath) { host = active; idx = i; break; }
            }
        }
        if (host == nullptr) {                
            for (Playlist* pl : playlists) {
                for (int i = 0; i < pl->getSongCount(); ++i) {
                    if (pl->getSong(i)->getFilePath() == lastPath) { host = pl; idx = i; break; }
                }
                if (host != nullptr) break;
            }
        }
        if (host != nullptr) player.prepareSong(host, idx);
    }
}

void Application::saveState() {
    config.set("playback_mode", player.getModeString());

    Playlist* active = player.getActivePlaylist();
    if (active != nullptr) config.set("active_playlist", active->getName());

  
    Song* last = player.getLastPlayedSong();
    config.set("last_song", last ? last->getFilePath() : config.get("last_song", ""));

    config.save();
}

void Application::cleanupPlaylists() {
    for (Playlist* pl : playlists) delete pl;  
    playlists.clear();                         
}
