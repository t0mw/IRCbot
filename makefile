CC = g++
CFLAGS = -Wall -Wextra -std=c++11
SOURCES = ircbot.cpp main.cpp BotFunctor.cpp IRCParser/IRCParser.cpp
LIB = -lcurl -lws2_32
EXEC = IRCbot.exe
DEFINES = -DUW_CHUJNIA

all:
	$(CC) -O3 $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIB) $(DEFINES) -DVERBOSE

debug:
	$(CC) -g $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIB) $(DEFINES) -DVERBOSE -DSUPER_VERBOSE -DDEBUG

