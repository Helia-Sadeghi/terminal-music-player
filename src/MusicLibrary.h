#pragma once
#include <vector>
#include <string>
#include "Song.h"

class MusicLibrary {
private:
    std::vector<Song*> songs;   
    int nextId;

public:
    MusicLibrary();
    ~MusicLibrary();

    MusicLibrary(const MusicLibrary&) = delete;
    MusicLibrary& operator=(const MusicLibrary&) = delete;

    void addSong(Song* song);

    Song* getSongByPath(const std::string& filePath) const;
    Song* getSongById(int id) const;

    const std::vector<Song*>& getAllSongs() const;
    std::vector<Song*> filterByArtist(const std::string& artist) const;
    std::vector<Song*> filterByAlbum(const std::string& album) const;

    int getNextId();

    int getSongCount() const;
    bool isEmpty() const;

    void clear();  
};
