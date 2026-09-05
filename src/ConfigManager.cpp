#include "ConfigManager.h"
#include <fstream>
#include <iostream>

using namespace std;

static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

ConfigManager::ConfigManager(const string& filePath) : filePath(filePath) {}

bool ConfigManager::load() {
    ifstream file(filePath);
    if (!file.is_open()) {
        return false;  
    }

    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == string::npos) continue;

        string key = trim(line.substr(0, pos));
        string value = trim(line.substr(pos + 1));
        if (!key.empty()) data[key] = value;
    }
    return true;
}

bool ConfigManager::save() const {
    ofstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: cannot save config to " << filePath << "\n";
        return false;
    }
    for (const auto& pair : data) {
        file << pair.first << "=" << pair.second << "\n";
    }
    return true;
}

string ConfigManager::get(const string& key, const string& defaultValue) const {
    auto it = data.find(key);
    return (it == data.end()) ? defaultValue : it->second;
}

void ConfigManager::set(const string& key, const string& value) {
    data[key] = value;
}

bool ConfigManager::hasKey(const string& key) const {
    return data.find(key) != data.end();
}
