#include "CsvLoader.h"
#include "Song.h"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool CsvLoader::load(const string& path, MusicLibrary& library) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Error: cannot open " << path << "\n";
        return false;
    }

    string line;
    int lineNo = 0;
    while (getline(file, line)) {
        ++lineNo;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;  

        if (lineNo == 1 && line.rfind("title,", 0) == 0) continue;

        stringstream ss(line);
        string title, artist, album, genre, yearStr, durationStr, filePath;

        getline(ss, title, ',');
        getline(ss, artist, ',');
        getline(ss, album, ',');
        getline(ss, genre, ',');
        getline(ss, yearStr, ',');
        getline(ss, durationStr, ',');
        getline(ss, filePath);          

        title       = trim(title);
        artist      = trim(artist);
        album       = trim(album);
        genre       = trim(genre);
        yearStr     = trim(yearStr);
        durationStr = trim(durationStr);
        filePath    = trim(filePath);

        if (title.empty() || filePath.empty()) {
            cerr << "Warning: skipping invalid line " << lineNo << "\n";
            continue;
        }

        int year = 0, duration = 0;
        try {
            if (!yearStr.empty())     year = stoi(yearStr);
            if (!durationStr.empty()) duration = stoi(durationStr);
        } catch (const exception&) {
            cerr << "Warning: skipping line " << lineNo
                 << " (invalid number)\n";
            continue;
        }

        int id = library.getNextId();
        Song* song = new Song(id, title, artist, album, genre,
                              year, duration, filePath);
        if (song->isValid()) {
            library.addSong(song);
        } else {
            delete song;
            cerr << "Warning: skipping invalid song on line " << lineNo << "\n";
        }
    }
    return true;
}
