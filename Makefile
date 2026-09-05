# Terminal Music Player - build file
# Run `make` from the Music_Player/ directory, then `./music_player`.

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
SRC_DIR  := src
SOURCES  := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  := $(SOURCES:.cpp=.o)
TARGET   := music_player

# Platform-specific libraries required by miniaudio.
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS := -lpthread -lm -ldl
endif
ifeq ($(UNAME_S),Darwin)
    LDFLAGS := -framework CoreFoundation -framework CoreAudio \
               -framework AudioToolbox -lpthread -lm
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
