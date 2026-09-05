#define MINIAUDIO_IMPLEMENTATION

#define MA_NO_JACK
#include "miniaudio.h"

#include "Player.h"
#include <cstdlib>
#include <chrono>
#include <filesystem>

using namespace std;

struct AudioBackend {
    ma_engine engine;
    ma_sound  sound;
    bool      engineReady = false;
};

Player::Player()
    : state(PlayerState::STOPPED),
      mode(PlaybackMode::NO_REPEAT),
      currentPlaylist(nullptr),
      activePlaylist(nullptr),
      currentIndex(-1),
      lastPlayedSong(nullptr),
      audio(new AudioBackend()),
      soundLoaded(false),
      simElapsed(0.0),
      lastTickTime(0.0),
      audioBaseDir("Data/") {
    if (ma_engine_init(nullptr, &audio->engine) == MA_SUCCESS) {
        audio->engineReady = true;
    }
    srand(static_cast<unsigned>(time(nullptr)));
    lastTickTime = nowSeconds();
}

Player::~Player() {
    unloadSound();
    if (audio) {
        if (audio->engineReady) ma_engine_uninit(&audio->engine);
        delete audio;
        audio = nullptr;
    }
}

double Player::nowSeconds() const {
    using namespace std::chrono;
    return duration_cast<duration<double>>(
               steady_clock::now().time_since_epoch()).count();
}

void Player::setAudioBaseDir(const string& dir) { audioBaseDir = dir; }

string Player::resolvePath(const Song* song) const {
    if (song == nullptr) return "";
    return audioBaseDir + song->getFilePath();
}

void Player::unloadSound() {
    if (audio && audio->engineReady && soundLoaded) {
        ma_sound_uninit(&audio->sound);
    }
    soundLoaded = false;
}

void Player::loadCurrent() {
    unloadSound();
    simElapsed = 0.0;
    lastTickTime = nowSeconds();

    Song* song = getCurrentSong();
    if (song == nullptr || !audio->engineReady) return;


    string path = resolvePath(song);
    error_code ec;
    if (!std::filesystem::exists(path, ec) ||
        !std::filesystem::is_regular_file(path, ec)) {
        return;
    }

    ma_result r = ma_sound_init_from_file(
        &audio->engine, path.c_str(), MA_SOUND_FLAG_DECODE,
        nullptr, nullptr, &audio->sound);
    soundLoaded = (r == MA_SUCCESS);
}

bool Player::play(Playlist* playlist, int index) {
    if (playlist == nullptr || playlist->isEmpty()) return false;
    if (index < 0 || index >= playlist->getSongCount()) return false;

    currentPlaylist = playlist;
    activePlaylist  = playlist;   
    currentIndex    = index;
    state           = PlayerState::PLAYING;

    loadCurrent();
    if (soundLoaded) ma_sound_start(&audio->sound);

    lastPlayedSong = getCurrentSong();
    return true;
}

void Player::prepareSong(Playlist* playlist, int index) {
    if (playlist == nullptr) return;
    if (index < 0 || index >= playlist->getSongCount()) return;

    currentPlaylist = playlist;
    currentIndex    = index;
    state           = PlayerState::STOPPED;  

    loadCurrent();                           
    lastPlayedSong = getCurrentSong();
}

void Player::pause() {
    if (state != PlayerState::PLAYING) return;     
    state = PlayerState::PAUSED;
    if (soundLoaded) ma_sound_stop(&audio->sound);  
}

void Player::resume() {
    if (state != PlayerState::PAUSED) return;  
    state = PlayerState::PLAYING;
    lastTickTime = nowSeconds();
    if (soundLoaded) ma_sound_start(&audio->sound);
}

void Player::stop() {
    if (state == PlayerState::STOPPED) return;
    state = PlayerState::STOPPED;
    simElapsed = 0.0;
    if (soundLoaded) {
        ma_sound_stop(&audio->sound);
        ma_sound_seek_to_pcm_frame(&audio->sound, 0);
    }
}

bool Player::next() {
    if (currentPlaylist == nullptr) return false;
    int idx = getNextIndex();
    if (idx == -1) {           
        stop();
        return false;
    }
    currentIndex = idx;
    state = PlayerState::PLAYING;
    loadCurrent();
    if (soundLoaded) ma_sound_start(&audio->sound);
    lastPlayedSong = getCurrentSong();
    return true;
}

bool Player::previous() {
    if (currentPlaylist == nullptr) return false;
    int idx = getPrevIndex();
    if (idx == -1) return false;
    currentIndex = idx;
    state = PlayerState::PLAYING;
    loadCurrent();
    if (soundLoaded) ma_sound_start(&audio->sound);
    lastPlayedSong = getCurrentSong();
    return true;
}

void Player::tick() {
    double now = nowSeconds();
    double delta = now - lastTickTime;
    lastTickTime = now;

    if (state != PlayerState::PLAYING) return;

    if (soundLoaded) {
        if (ma_sound_at_end(&audio->sound)) {
            next();  
        }
    } else {

        simElapsed += delta;
        Song* song = getCurrentSong();
        if (song != nullptr && song->getDuration() > 0 &&
            simElapsed >= song->getDuration()) {
            next();
        }
    }
}

void Player::seekBy(int seconds) {
    if (state == PlayerState::STOPPED) return;
    if (soundLoaded && audio->engineReady) {
        ma_uint64 cursor = 0, length = 0;
        ma_sound_get_cursor_in_pcm_frames(&audio->sound, &cursor);
        ma_sound_get_length_in_pcm_frames(&audio->sound, &length);
        ma_uint32 rate = ma_engine_get_sample_rate(&audio->engine);
        long long newFrame = static_cast<long long>(cursor) +
                             static_cast<long long>(seconds) * rate;
        if (newFrame < 0) newFrame = 0;
        if (length > 0 && static_cast<ma_uint64>(newFrame) >= length) {
            next();
            return;
        }
        ma_sound_seek_to_pcm_frame(&audio->sound,
                                   static_cast<ma_uint64>(newFrame));
    } else {
  
        simElapsed += seconds;
        if (simElapsed < 0) simElapsed = 0;
        Song* s = getCurrentSong();
        if (s != nullptr && s->getDuration() > 0 &&
            simElapsed >= s->getDuration()) {
            next();
        }
    }
}

int Player::getNextIndex() const {
    if (currentPlaylist == nullptr) return -1;
    int count = currentPlaylist->getSongCount();
    if (count == 0 || currentIndex < 0 || currentIndex >= count) return -1;

    switch (mode) {
        case PlaybackMode::NO_REPEAT:
            return (currentIndex + 1 < count) ? currentIndex + 1 : -1;
        case PlaybackMode::REPEAT_ONE:
            return currentIndex;
        case PlaybackMode::REPEAT_ALL:
            return (currentIndex + 1) % count;
        case PlaybackMode::SHUFFLE: {
            if (count <= 1) return currentIndex;
            int r;
            do { r = rand() % count; } while (r == currentIndex);
            return r;
        }
    }
    return -1;
}

int Player::getPrevIndex() const {
    if (currentPlaylist == nullptr) return -1;
    int count = currentPlaylist->getSongCount();
    if (count == 0 || currentIndex < 0 || currentIndex >= count) return -1;

    switch (mode) {
        case PlaybackMode::NO_REPEAT:
            return (currentIndex - 1 >= 0) ? currentIndex - 1 : -1;
        case PlaybackMode::REPEAT_ONE:
            return currentIndex;
        case PlaybackMode::REPEAT_ALL:
            return (currentIndex - 1 + count) % count;
        case PlaybackMode::SHUFFLE: {
            if (count <= 1) return currentIndex;
            int r;
            do { r = rand() % count; } while (r == currentIndex);
            return r;
        }
    }
    return -1;
}

void Player::setActivePlaylist(Playlist* pl) { activePlaylist = pl; }
Playlist* Player::getActivePlaylist() const { return activePlaylist; }

void Player::switchActivePlaylist(Playlist* pl) {

    stop();
    unloadSound();
    currentPlaylist = nullptr;
    currentIndex = -1;
    state = PlayerState::STOPPED;
    activePlaylist = pl;
}

PlayerState  Player::getState() const { return state; }
PlaybackMode Player::getMode() const { return mode; }
int          Player::getCurrentIndex() const { return currentIndex; }
Playlist*    Player::getCurrentPlaylist() const { return currentPlaylist; }
Song*        Player::getLastPlayedSong() const { return lastPlayedSong; }

Song* Player::getCurrentSong() const {
    if (currentPlaylist == nullptr || currentIndex < 0) return nullptr;
    return currentPlaylist->getSong(currentIndex);
}

void Player::setMode(PlaybackMode newMode) { mode = newMode; }

bool Player::isPlaying() const { return state == PlayerState::PLAYING; }
bool Player::isPaused() const { return state == PlayerState::PAUSED; }
bool Player::isStopped() const { return state == PlayerState::STOPPED; }
bool Player::isAudioLoaded() const { return soundLoaded; }

string Player::getStateString() const {
    switch (state) {
        case PlayerState::PLAYING: return "PLAYING";
        case PlayerState::PAUSED:  return "PAUSED";
        case PlayerState::STOPPED: return "STOPPED";
    }
    return "";
}

string Player::getModeString() const {
    switch (mode) {
        case PlaybackMode::NO_REPEAT:  return "NO_REPEAT";
        case PlaybackMode::REPEAT_ONE: return "REPEAT_ONE";
        case PlaybackMode::REPEAT_ALL: return "REPEAT_ALL";
        case PlaybackMode::SHUFFLE:    return "SHUFFLE";
    }
    return "";
}

double Player::getCursorSec() const {
    if (soundLoaded && audio->engineReady) {
        ma_uint64 frames = 0;
        ma_sound_get_cursor_in_pcm_frames(
            const_cast<ma_sound*>(&audio->sound), &frames);
        ma_uint32 rate = ma_engine_get_sample_rate(
            const_cast<ma_engine*>(&audio->engine));
        if (rate > 0) return static_cast<double>(frames) / rate;
    }
    return simElapsed;
}

double Player::getLengthSec() const {
    if (soundLoaded && audio->engineReady) {
        ma_uint64 frames = 0;
        if (ma_sound_get_length_in_pcm_frames(&audio->sound, &frames) == MA_SUCCESS) {
            ma_uint32 rate = ma_engine_get_sample_rate(
                const_cast<ma_engine*>(&audio->engine));
            if (rate > 0) return static_cast<double>(frames) / rate;
        }
    }
    Song* s = getCurrentSong();
    return s ? s->getDuration() : 0.0;
}
