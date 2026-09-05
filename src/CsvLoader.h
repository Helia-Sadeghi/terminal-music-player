#pragma once
#include <string>
#include "MusicLibrary.h"

class CsvLoader {
public:
    static bool load(const std::string& path, MusicLibrary& library);
};
