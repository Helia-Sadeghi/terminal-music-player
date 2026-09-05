#pragma once
#include <string>

enum KeyCode {
    KEY_NONE  = -1,
    KEY_LEFT  = 1000,
    KEY_RIGHT = 1001
};


class InputHandler {
public:

    int readInt(int min, int max, const std::string& prompt,
                const std::string& errMsg = "Invalid choice. Please try again.");


    std::string readLine(const std::string& prompt = "");

    char readKey(const std::string& prompt = "");


    void enableRawMode();
    void disableRawMode();

    int  pollKeyCode();
};
