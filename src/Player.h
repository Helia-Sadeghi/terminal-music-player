#pragma once
#include <string>
#include "Playlist.h"
#include "Song.h"

enum class PlayerState {
    STOPPED,
    PLAYING,
    PAUSED
};

enum class PlaybackMode {
    NO_REPEAT,
    REPEAT_ONE,
    REPEAT_ALL,
    SHUFFLE
};

struct AudioBackend;

class Player {
private:
    PlayerState  state;
    PlaybackMode mode;

    Playlist* currentPlaylist;   
    Playlist* activePlaylist;    
    int       currentIndex;
    Song*     lastPlayedSong;   

    AudioBackend* audio;        
    bool   soundLoaded;
    double simElapsed;         
    double lastTickTime;        

    std::string audioBaseDir;   

    int  getNextIndex() const;
    int  getPrevIndex() const;
    void loadCurrent();        
    void unloadSound();
    double nowSeconds() const;
    std::string resolvePath(const Song* song) const;

public:
    Player();
    ~Player();

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    void setAudioBaseDir(const std::string& dir);

    bool play(Playlist* playlist, int index = 0);

    void prepareSong(Playlist* playlist, int index);
    void pause();
    void resume();
    void stop();
    bool next();
    bool previous();
    void tick();

    void seekBy(int seconds);

  
    void      setActivePlaylist(Playlist* pl);
    Playlist* getActivePlaylist() const;
    void      switchActivePlaylist(Playlist* pl);   

    // Getters
    PlayerState  getState() const;
    PlaybackMode getMode() const;
    Song*        getCurrentSong() const;
    Playlist*    getCurrentPlaylist() const;
    int          getCurrentIndex() const;
    Song*        getLastPlayedSong() const;

    void setMode(PlaybackMode mode);

    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;
    bool isAudioLoaded() const;

    std::string getStateString() const;
    std::string getModeString() const;

    double getCursorSec() const; 
    double getLengthSec() const;   
};
