#pragma once
#include <string>
#include <map>

class ConfigManager {
private:
    std::string filePath;
    std::map<std::string, std::string> data;

public:
    explicit ConfigManager(const std::string& filePath);

    bool load();
    bool save() const;

    std::string get(const std::string& key,
                    const std::string& defaultValue = "") const;
    void set(const std::string& key, const std::string& value);
    bool hasKey(const std::string& key) const;
};
