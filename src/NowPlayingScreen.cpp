#include "NowPlayingScreen.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <chrono>
#include <thread>

#ifdef _WIN32
  #include <io.h>
  static bool stdinIsTty() { return _isatty(_fileno(stdin)) != 0; }
#else
  #include <unistd.h>
  static bool stdinIsTty() { return isatty(STDIN_FILENO) != 0; }
#endif

using namespace std;

NowPlayingScreen::NowPlayingScreen(UIRenderer& renderer, InputHandler& input,
                                   Player& player)
    : Screen(renderer, input), player(player) {}

string NowPlayingScreen::fmt(double s) {
    if (s < 0) s = 0;
    int total = static_cast<int>(s);
    int m = total / 60, sec = total % 60;
    stringstream ss;
    ss << setfill('0') << setw(2) << m << ":" << setw(2) << sec;
    return ss.str();
}

void NowPlayingScreen::draw() {
    renderer.drawTopBorder();
    renderer.drawCenteredRow("\u266B Terminal Music Player \u266B");
    renderer.drawSeparator();
    renderer.drawRow("Now Playing");

    Song* song = player.getCurrentSong();
    if (song == nullptr) {
        renderer.drawSeparator();
        renderer.drawRow("Nothing is playing right now.");
        renderer.drawRow("Open Playlists or Browse to start a song.");
    } else {
        renderer.drawRow("Title  : " + song->getTitle());
        renderer.drawRow("Artist : " + song->getArtist());
        renderer.drawRow("Album  : " + song->getAlbum() +
                         " [" + to_string(song->getYear()) + "]");
        renderer.drawRow("Genre  : " + song->getGenre());
        renderer.drawSeparator();

        string sym = player.isPlaying() ? "\u25B6 PLAYING"
                   : player.isPaused()  ? "\u23F8 PAUSED"
                                        : "\u25A0 STOPPED";
        string pl = player.getCurrentPlaylist()
                        ? player.getCurrentPlaylist()->getName() : "-";

        renderer.drawRow(sym + "    Playlist: " + pl);
        renderer.drawRow("Mode: " + player.getModeString() +
                         "    Duration: " + song->getDurationString());
        renderer.drawRow("Elapsed: " + fmt(player.getCursorSec()) +
                         " / " + fmt(player.getLengthSec()));
    }

    renderer.drawSeparator();
    renderer.drawRow("[p] pause/resume [n] next [b] prev [s] stop [q] menu");
    renderer.drawRow("[<- ->] seek 10s");
    renderer.drawBottomBorder();
}

void NowPlayingScreen::render() {
    renderer.clearScreen();
    draw();
}

int NowPlayingScreen::handleInput() {

    if (!stdinIsTty()) {
        player.tick();
        string line = input.readLine("Choice: ");
        if (cin.eof()) return NAV_BACK;
        int key = line.empty() ? '\n' : (unsigned char)line[0];
        if (processKey(key)) return NAV_BACK;   
        return NAV_STAY;                        
    }

    input.enableRawMode();
    renderer.hideCursor();

    int lastShownSec = -1;
    string lastState = "";

    while (true) {
        player.tick();

        int curSec = static_cast<int>(player.getCursorSec());
        string st = player.getStateString();
        if (curSec != lastShownSec || st != lastState) {
            renderer.home();
            draw();
            lastShownSec = curSec;
            lastState = st;
        }

        int key = input.pollKeyCode();
        if (key != KEY_NONE) {
            if (processKey(key)) break;  
            lastShownSec = -1;            
        }

        this_thread::sleep_for(chrono::milliseconds(80));
    }

    renderer.showCursor();
    input.disableRawMode();
    return NAV_BACK;
}

bool NowPlayingScreen::processKey(int key) {
    int k = (key >= 0 && key < 256) ? tolower(key) : key;
    if (k == 'q') return true;
    else if (k == 'p') { if (player.isPlaying()) player.pause();
                         else player.resume(); }
    else if (k == 'n') player.next();
    else if (k == 'b') player.previous();
    else if (k == 's') player.stop();
    else if (key == KEY_RIGHT) player.seekBy(10);
    else if (key == KEY_LEFT)  player.seekBy(-10);
    return false;
}
