#include "InputHandler.h"
#include <iostream>
#include <limits>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;

#ifndef _WIN32
static termios g_oldTerm;
static bool    g_rawActive = false;
#endif

int InputHandler::readInt(int minV, int maxV, const string& prompt,
                          const string& errMsg) {
    while (true) {
        if (!prompt.empty()) cout << prompt;
        int value;
        if (!(cin >> value)) {
            if (cin.eof()) return minV;   // stdin closed: leave gracefully
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << errMsg << "\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (value < minV || value > maxV) {
            cout << errMsg << "\n";
            continue;
        }
        return value;
    }
}

string InputHandler::readLine(const string& prompt) {
    if (!prompt.empty()) cout << prompt;
    string line;
    getline(cin, line);

    size_t first = line.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = line.find_last_not_of(" \t\r\n");
    return line.substr(first, last - first + 1);
}

char InputHandler::readKey(const string& prompt) {
    string line = readLine(prompt);
    return line.empty() ? '\0' : line[0];
}

void InputHandler::enableRawMode() {
#ifndef _WIN32
    if (g_rawActive) return;
    tcgetattr(STDIN_FILENO, &g_oldTerm);
    termios raw = g_oldTerm;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;   // non-blocking read
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    g_rawActive = true;
#endif
}

void InputHandler::disableRawMode() {
#ifndef _WIN32
    if (!g_rawActive) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &g_oldTerm);
    g_rawActive = false;
#endif
}

int InputHandler::pollKeyCode() {
#ifdef _WIN32
    if (!_kbhit()) return KEY_NONE;
    int c = _getch();
    if (c == 0 || c == 224) {           // arrow-key prefix on Windows
        int code = _getch();
        if (code == 75) return KEY_LEFT;
        if (code == 77) return KEY_RIGHT;
        return KEY_NONE;
    }
    return c;
#else
    unsigned char c;
    int n = read(STDIN_FILENO, &c, 1);
    if (n <= 0) return KEY_NONE;
    if (c == 27) {                       // ESC: maybe an arrow sequence
        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) <= 0) return 27;
        if (read(STDIN_FILENO, &seq[1], 1) <= 0) return 27;
        if (seq[0] == '[') {
            if (seq[1] == 'C') return KEY_RIGHT;
            if (seq[1] == 'D') return KEY_LEFT;
        }
        return KEY_NONE;
    }
    return c;
#endif
}
