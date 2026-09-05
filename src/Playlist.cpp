#include "Playlist.h"

using namespace std;

Playlist::Playlist(const string& name) : name(name) {}

string Playlist::getName() const { return name; }

int Playlist::getSongCount() const {
    return static_cast<int>(songs.size());
}

bool Playlist::isEmpty() const { return songs.empty(); }

const vector<Song*>& Playlist::getSongs() const { return songs; }

Song* Playlist::getSong(int index) const {
    if (index < 0 || index >= static_cast<int>(songs.size())) {
        return nullptr;
    }
    return songs[index];
}

int Playlist::indexOf(const Song* song) const {
    for (int i = 0; i < static_cast<int>(songs.size()); ++i) {
        if (songs[i] == song) return i;
    }
    return -1;
}

void Playlist::addSong(Song* song) {
    if (song != nullptr) {
        songs.push_back(song);
    }
}

bool Playlist::hasSong(const string& filePath) const {
    for (Song* song : songs) {
        if (song != nullptr && song->getFilePath() == filePath) {
            return true;
        }
    }
    return false;
}

void Playlist::clear() {
    songs.clear();   
}
