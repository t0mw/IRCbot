CC = g++
CFLAGS = -Wall -Wextra -std=c++11
MODULES = modules/PrintWWWTitle.cpp modules/WWWDownload.cpp modules/UserMemo.cpp
SOURCES = IRCBot.cpp main.cpp BotFunctor.cpp IRCParser/IRCParser.cpp $(MODULES)
LIB = -L"C:\\MinGW\\bin" -lcurl -lws2_32
EXEC = IRCbot.exe
DEFINES = -DVERBOSE
DEBUG_DEFINES = -DVERBOSE -DSUPER_VERBOSE -DDEBUG

all:
	$(CC) -O3 $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIB) $(DEFINES)

debug:
	$(CC) -g $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIB) $(DEFINES) $(DEBUG_DEFINES)
