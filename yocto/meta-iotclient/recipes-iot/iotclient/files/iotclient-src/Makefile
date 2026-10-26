# Yocto sets CXX / CXXFLAGS / LDFLAGS in the environment, so keep them overridable
CXX      ?= g++
CXXFLAGS ?= -O2
override CXXFLAGS += -std=c++17 -Wall -Wextra
LDFLAGS  += -pthread

COMMON_SRC := common/TCPSocket.cpp common/UDPSocket.cpp
COMMON_HDR := $(wildcard common/*.hpp)

QT_PREFIX ?= $(HOME)/Qt/6.8.3/gcc_64
GUI_DIR   := server/gui
GUI_BUILD := $(GUI_DIR)/build-make

.PHONY: all client console gui clean

all: client console gui

client:  client_app
console: server_console

client_app: client/main.cpp $(COMMON_SRC) $(COMMON_HDR)
	$(CXX) $(CXXFLAGS) -o $@ client/main.cpp $(COMMON_SRC) $(LDFLAGS)

server_console: server/console/console_main.cpp $(COMMON_SRC) $(COMMON_HDR)
	$(CXX) $(CXXFLAGS) -o $@ server/console/console_main.cpp $(COMMON_SRC) $(LDFLAGS)

gui:
	cmake -S $(GUI_DIR) -B $(GUI_BUILD) -DCMAKE_PREFIX_PATH=$(QT_PREFIX)
	cmake --build $(GUI_BUILD) -j$$(nproc)

clean:
	rm -f client_app server_console
	rm -rf $(GUI_BUILD)
