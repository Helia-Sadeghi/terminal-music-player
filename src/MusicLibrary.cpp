#include "MusicLibrary.h"
#include <cctype>

using namespace std;

static bool equalsIgnoreCase(const string& a, const string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (tolower(static_cast<unsigned char>(a[i])) !=
            tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

MusicLibrary::MusicLibrary() : nextId(1) {}

MusicLibrary::~MusicLibrary() { clear(); }

void MusicLibrary::addSong(Song* song) {
    if (song != nullptr) songs.push_back(song);
}

Song* MusicLibrary::getSongByPath(const string& filePath) const {
    for (Song* song : songs) {
        if (song != nullptr && song->getFilePath() == filePath) return song;
    }
    return nullptr;
}

Song* MusicLibrary::getSongById(int id) const {
    for (Song* song : songs) {
        if (song != nullptr && song->getId() == id) return song;
    }
    return nullptr;
}

const vector<Song*>& MusicLibrary::getAllSongs() const { return songs; }

vector<Song*> MusicLibrary::filterByArtist(const string& artist) const {
    vector<Song*> result;
    for (Song* song : songs) {
        if (song != nullptr && equalsIgnoreCase(song->getArtist(), artist)) {
            result.push_back(song);
        }
    }
    return result;
}

vector<Song*> MusicLibrary::filterByAlbum(const string& album) const {
    vector<Song*> result;
    for (Song* song : songs) {
        if (song != nullptr && equalsIgnoreCase(song->getAlbum(), album)) {
            result.push_back(song);
        }
    }
    return result;
}

int MusicLibrary::getNextId() { return nextId++; }

int MusicLibrary::getSongCount() const {
    return static_cast<int>(songs.size());
}

bool MusicLibrary::isEmpty() const { return songs.empty(); }

void MusicLibrary::clear() {
    for (Song* song : songs) delete song;   // sole owner frees the Songs
    songs.clear();
}
