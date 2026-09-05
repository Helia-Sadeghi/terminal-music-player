#pragma once
#include <string>
#include <vector>
#include "MusicLibrary.h"
#include "Playlist.h"

class M3uLoader {
public:
    static void loadAll(const std::string& dir,
                        const MusicLibrary& library,
                        std::vector<Playlist*>& playlists);

    static Playlist* load(const std::string& filePath,
                          const MusicLibrary& library);
};
