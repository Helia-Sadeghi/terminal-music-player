#pragma once
#include "Screen.h"
#include "Player.h"
#include "Playlist.h"
#include <string>
#include <vector>

class BrowsePlaylistScreen : public Screen {
private:
    Player& player;

    enum SortKey { S_NONE, S_TITLE, S_ARTIST, S_ALBUM, S_YEAR, S_DURATION };
    SortKey sortKey;
    bool    sortDesc;

    bool        searchMode;
    std::string searchQuery;

    enum FilterType { F_NONE, F_ARTIST, F_ALBUM };
    FilterType  filterType;
    std::string filterValue;

    std::vector<Song*> buildList() const;   
    void openSortMenu();
    void openFilterMenu();
    void openSearchPrompt();

public:
    BrowsePlaylistScreen(UIRenderer& renderer, InputHandler& input,
                         Player& player);

    void render() override;
    int  handleInput() override;
};
