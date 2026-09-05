#include "M3uLoader.h"
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void M3uLoader::loadAll(const string& dir, const MusicLibrary& library,
                        vector<Playlist*>& playlists) {
    error_code ec;
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) {
        return;  
    }

    vector<string> files;
    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (!entry.is_regular_file()) continue;
        string ext = entry.path().extension().string();
        transform(ext.begin(), ext.end(), ext.begin(),
                  [](unsigned char c){ return (char)tolower(c); });
        if (ext == ".m3u") {
            files.push_back(entry.path().string());
        }
    }
    sort(files.begin(), files.end());

    for (const string& fullPath : files) {
        Playlist* pl = load(fullPath, library);
        if (pl != nullptr) {
            playlists.push_back(pl);
        }
    }
}

Playlist* M3uLoader::load(const string& filePath, const MusicLibrary& library) {
    ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }

    string name = fs::path(filePath).stem().string();

    Playlist* playlist = new Playlist(name);

    string line;
    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();   // CRLF
        if (line.empty() || line[0] == '#') continue;                // comments

        Song* song = library.getSongByPath(line);
        if (song != nullptr) {
            playlist->addSong(song);
        }
    }

    if (playlist->isEmpty()) {
        delete playlist;
        return nullptr;
    }
    return playlist;
}
