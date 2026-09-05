#include "BrowsePlaylistScreen.h"
#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;

static int udisp(const string& s) {
    int w = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) ++w;
    return w;
}
static string utake(const string& s, int width) {
    int count = 0; size_t i = 0;
    while (i < s.size() && count < width) {
        unsigned char c = s[i];
        size_t len = 1;
        if      ((c & 0x80) == 0x00) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;
        if (i + len > s.size()) len = s.size() - i;
        i += len; ++count;
    }
    return s.substr(0, i);
}
static string upad(const string& s, int width) {
    int w = udisp(s);
    if (w > width) {
        if (width <= 3) return utake(s, width);
        return utake(s, width - 3) + "...";
    }
    return s + string(width - w, ' ');
}
static string lower(const string& s) {
    string r = s;
    for (char& c : r) c = (char)tolower((unsigned char)c);
    return r;
}
static bool icontains(const string& hay, const string& needle) {
    if (needle.empty()) return true;
    return lower(hay).find(lower(needle)) != string::npos;
}
static bool iequals(const string& a, const string& b) {
    return lower(a) == lower(b);
}

BrowsePlaylistScreen::BrowsePlaylistScreen(UIRenderer& renderer,
                                           InputHandler& input, Player& player)
    : Screen(renderer, input), player(player),
      sortKey(S_NONE), sortDesc(false),
      searchMode(false), searchQuery(""),
      filterType(F_NONE), filterValue("") {}

vector<Song*> BrowsePlaylistScreen::buildList() const {
    vector<Song*> list;
    Playlist* pl = player.getActivePlaylist();
    if (pl == nullptr) return list;

    for (Song* s : pl->getSongs()) {
        if (s == nullptr) continue;
        if (filterType == F_ARTIST && !iequals(s->getArtist(), filterValue)) continue;
        if (filterType == F_ALBUM  && !iequals(s->getAlbum(),  filterValue)) continue;
        if (searchMode && !searchQuery.empty()) {
            if (!(icontains(s->getTitle(),  searchQuery) ||
                  icontains(s->getArtist(), searchQuery) ||
                  icontains(s->getAlbum(),  searchQuery))) continue;
        }
        list.push_back(s);
    }

    if (sortKey != S_NONE) {
        auto cmp = [this](Song* a, Song* b) {
            int c = 0;   // -1, 0, +1
            switch (sortKey) {
                case S_TITLE: {
                    string la = lower(a->getTitle()), lb = lower(b->getTitle());
                    c = (la > lb) - (la < lb); break;
                }
                case S_ARTIST: {
                    string la = lower(a->getArtist()), lb = lower(b->getArtist());
                    c = (la > lb) - (la < lb); break;
                }
                case S_ALBUM: {
                    string la = lower(a->getAlbum()), lb = lower(b->getAlbum());
                    c = (la > lb) - (la < lb); break;
                }
                case S_YEAR:
                    c = (a->getYear() > b->getYear()) - (a->getYear() < b->getYear());
                    break;
                case S_DURATION:
                    c = (a->getDuration() > b->getDuration()) -
                        (a->getDuration() < b->getDuration());
                    break;
                default: c = 0; break;
            }
            if (c == 0) return false;         
            return sortDesc ? c > 0 : c < 0;
        };
        stable_sort(list.begin(), list.end(), cmp);
    }
    return list;
}

void BrowsePlaylistScreen::render() {
    renderer.clearScreen();
    renderer.drawTopBorder();

    Playlist* pl = player.getActivePlaylist();
    if (pl == nullptr) {
        renderer.drawRow("Browse Playlist");
        renderer.drawSeparator();
        renderer.drawRow("No active playlist. Choose one from Playlists (menu 2).");
        renderer.drawSeparator();
        renderer.drawRow("0. Back");
        renderer.drawBottomBorder();
        return;
    }

    vector<Song*> list = buildList();

    string left = pl->getName() + " (" + to_string(pl->getSongCount()) + " songs)";
    string right;
    if (searchMode) {
        right = "Search: \"" + searchQuery + "\"";
    } else if (sortKey != S_NONE) {
        const char* names[] = {"None","Title","Artist","Album","Year","Dur"};
        right = string("Sort: ") + names[sortKey] +
                (sortDesc ? " \u2193" : " \u2191");
    }
    int total = renderer.textWidth();
    int gap = total - udisp(left) - udisp(right);
    if (gap < 1) gap = 1;
    renderer.drawRow(left + string(gap, ' ') + right);
    renderer.drawSeparator();

    // Column header
    renderer.drawRow(string("  ") + upad("#", 3) + " " + upad("Title", 24) +
                     " " + upad("Artist", 16) + " " + "Dur");
    renderer.drawSeparator();

    if (list.empty()) {
        renderer.drawRow(searchMode ? "No matches." : "Playlist is empty.");
    } else {
        Song* playing = player.getCurrentSong();
        for (int i = 0; i < static_cast<int>(list.size()); ++i) {
            Song* s = list[i];
            string mark = (s == playing) ? "\u25B6 " : "  ";
            string num = to_string(i + 1) + ".";
            string row = mark + upad(num, 3) + " " +
                         upad(s->getTitle(), 24) + " " +
                         upad(s->getArtist(), 16) + " " +
                         s->getDurationString();
            renderer.drawRow(row);
        }
    }

    renderer.drawSeparator();
    if (searchMode) {
        renderer.drawRow(to_string(list.size()) +
                         " result(s). [num] play  [/] new search  [0] clear");
    } else {
        renderer.drawRow("[num] play song  [s] sort  [f] filter  [/] search");
        renderer.drawRow("[0] back");
    }
    renderer.drawBottomBorder();
}

void BrowsePlaylistScreen::openSortMenu() {
    renderer.drawSeparator();
    renderer.drawRow("Sort by: 1.Title 2.Artist 3.Album 4.Year 5.Dur");
    renderer.drawRow("Add + for descending (e.g. 4+ for Year desc)");
    string in = input.readLine("Sort choice: ");
    if (in.empty()) return;

    bool desc = (in.back() == '+');
    if (desc) in.pop_back();
    if (in.empty() || !isdigit((unsigned char)in[0])) return;

    int n = in[0] - '0';
    switch (n) {
        case 1: sortKey = S_TITLE;    break;
        case 2: sortKey = S_ARTIST;   break;
        case 3: sortKey = S_ALBUM;    break;
        case 4: sortKey = S_YEAR;     break;
        case 5: sortKey = S_DURATION; break;
        default: return;
    }
    sortDesc = desc;
}

void BrowsePlaylistScreen::openSearchPrompt() {
    string q = input.readLine("Search: ");
    if (q.empty()) {               // empty query clears the search
        searchMode = false;
        searchQuery = "";
    } else {
        searchMode = true;
        searchQuery = q;
    }
}

void BrowsePlaylistScreen::openFilterMenu() {
    Playlist* pl = player.getActivePlaylist();
    if (pl == nullptr) return;

    renderer.clearScreen();
    renderer.drawTopBorder();
    renderer.drawRow("Filter songs in: " + pl->getName());
    renderer.drawSeparator();
    renderer.drawRow("Filter by:");
    renderer.drawRow("1. Artist");
    renderer.drawRow("2. Album");
    renderer.drawSeparator();
    renderer.drawRow("0. Back");
    renderer.drawBottomBorder();

    int choice = input.readInt(0, 2, "Choice: ");
    if (choice == 0) return;

    bool byArtist = (choice == 1);

    vector<pair<string,int>> values;
    for (Song* s : pl->getSongs()) {
        if (s == nullptr) continue;
        string v = byArtist ? s->getArtist() : s->getAlbum();
        bool found = false;
        for (auto& p : values) {
            if (iequals(p.first, v)) { p.second++; found = true; break; }
        }
        if (!found) values.push_back({v, 1});
    }
    sort(values.begin(), values.end(),
         [](const pair<string,int>& a, const pair<string,int>& b) {
             return lower(a.first) < lower(b.first);
         });

    renderer.clearScreen();
    renderer.drawTopBorder();
    renderer.drawRow((byArtist ? "Artists in " : "Albums in ") + pl->getName());
    renderer.drawSeparator();
    for (int i = 0; i < static_cast<int>(values.size()); ++i) {
        renderer.drawRow(to_string(i + 1) + ". " + values[i].first +
                         " (" + to_string(values[i].second) + " songs)");
    }
    renderer.drawSeparator();
    renderer.drawRow("0. Back");
    renderer.drawBottomBorder();

    int sel = input.readInt(0, static_cast<int>(values.size()), "Choice: ");
    if (sel == 0) return;

    filterType  = byArtist ? F_ARTIST : F_ALBUM;
    filterValue = values[sel - 1].first;
    searchMode  = false;            
    searchQuery = "";
}

int BrowsePlaylistScreen::handleInput() {
    Playlist* pl = player.getActivePlaylist();
    if (pl == nullptr) {
        input.readInt(0, 0, "Choice: ");
        return NAV_BACK;
    }

    string in = input.readLine("Choice: ");
    if (in.empty()) return NAV_STAY;

    // Single-letter commands
    if (in == "s" && !searchMode) { openSortMenu();     return NAV_STAY; }
    if (in == "f" && !searchMode) { openFilterMenu();   return NAV_STAY; }
    if (in == "/")                { openSearchPrompt(); return NAV_STAY; }

    if (in == "0") {
        if (searchMode) {                 // clear search, stay
            searchMode = false; searchQuery = "";
            return NAV_STAY;
        }
        if (filterType != F_NONE) {       // clear filter -> full playlist view
            filterType = F_NONE; filterValue = "";
            return NAV_STAY;
        }
        return NAV_BACK;                  // leave to Main Menu
    }

    int num;
    try { num = stoi(in); }
    catch (...) { cout << "Invalid choice. Please try again.\n"; return NAV_STAY; }

    vector<Song*> list = buildList();
    if (list.empty()) {
        cout << "Playlist is empty.\n";
        return NAV_STAY;
    }
    if (num < 1 || num > static_cast<int>(list.size())) {
        cout << "No song at this index.\n";
        return NAV_STAY;
    }
    Song* chosen = list[num - 1];
    int realIdx = pl->indexOf(chosen);
    if (realIdx < 0) realIdx = 0;
    player.play(pl, realIdx);
    return NAV_NOW_PLAYING;
}
