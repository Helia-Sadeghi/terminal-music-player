# Terminal Music Player

A fully terminal-based (TUI) music player written in C++, implementing object-oriented design, dynamic memory management, CSV/M3U file parsing, and real audio playback via the miniaudio library.

## Features

- Play / pause / resume / stop / next / previous
- Repeat modes: NO_REPEAT, REPEAT_ONE, REPEAT_ALL, and SHUFFLE
- Switch between playlists
- Filter by artist or album
- Sort by title, artist, album, year, or duration (ascending/descending, display-only)
- Case-insensitive search
- Persistent settings saved to `settings.cfg`
- Automatic playback of the next track based on the active repeat mode

## Project Structure

```
Music_Player/
├── Makefile
├── src/ ← all .h/.cpp files + miniaudio.h
└── Data/
 ├── library.csv ← song metadata
 ├── settings.cfg ← generated at runtime
 ├── Playlists/*.m3u ← playlists
 └── Musics/*.mp3 ← audio files (add your own)
```

> **Note:** The program reads `Data/...` paths relative to the working directory, so it must be run from inside the `Music_Player/` folder.

## Build and Run

### Linux / macOS

```bash
cd Music_Player
make
./music_player
```

### Windows

With MinGW:

```bat
g++ -std=c++17 src\*.cpp -o music_player.exe
music_player.exe
```

Alternatively, open the project in Visual Studio and add all files from `src` (C++17). No external library linking is required — miniaudio is header-only.

## Real Audio Playback

The `MINIAUDIO_IMPLEMENTATION` macro is defined only in `Player.cpp`. To hear audio, place `.mp3` files in `Data/Musics/` so that their paths exactly match the `file_path` column in `library.csv`. If an audio file is missing, the program won't crash — it continues in simulation mode (silent, with a virtual timer).

## Controls

- Menus: option number + Enter, and `0` to go back.
- Now Playing screen: `p` pause/resume, `n` next, `b` previous, `s` stop, `q` return to menu, and arrow keys `←/→` for 10-second seek.