#pragma once
#include <string>

class Song {
private:
    int id;
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    int year;
    int duration;          
    std::string filePath;   

public:
    Song(int id, const std::string& title, const std::string& artist,
         const std::string& album, const std::string& genre,
         int year, int duration, const std::string& filePath);

    int getId() const;
    std::string getTitle() const;
    std::string getArtist() const;
    std::string getAlbum() const;
    std::string getGenre() const;
    int getYear() const;
    int getDuration() const;
    std::string getFilePath() const;

    std::string getDurationString() const;

    bool isValid() const;
};
