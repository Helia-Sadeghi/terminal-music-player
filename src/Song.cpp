#include "Song.h"
#include <iomanip>
#include <sstream>

using namespace std;

Song::Song(int id, const string& title, const string& artist,
           const string& album, const string& genre,
           int year, int duration, const string& filePath)
    : id(id), title(title), artist(artist), album(album),
      genre(genre), year(year), duration(duration), filePath(filePath) {}

int Song::getId() const { return id; }
string Song::getTitle() const { return title; }
string Song::getArtist() const { return artist; }
string Song::getAlbum() const { return album; }
string Song::getGenre() const { return genre; }
int Song::getYear() const { return year; }
int Song::getDuration() const { return duration; }
string Song::getFilePath() const { return filePath; }

string Song::getDurationString() const {
    int d = duration < 0 ? 0 : duration;
    int mins = d / 60;
    int secs = d % 60;
    stringstream ss;
    ss << setfill('0') << setw(2) << mins << ":"
       << setw(2) << secs;
    return ss.str();
}

bool Song::isValid() const {
    return !title.empty() && !filePath.empty();
}
