#pragma once
#include <string>
#include <vector>
#include "Song.h"

class Playlist {
private:
    std::string name;
    std::vector<Song*> songs;  

public:
    explicit Playlist(const std::string& name);

    Playlist(const Playlist&) = delete;
    Playlist& operator=(const Playlist&) = delete;

    std::string getName() const;
    int getSongCount() const;
    bool isEmpty() const;
    const std::vector<Song*>& getSongs() const;
    Song* getSong(int index) const;
    int indexOf(const Song* song) const;   

    void addSong(Song* song);
    bool hasSong(const std::string& filePath) const;
    void clear();   
};
