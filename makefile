CC = g++
CFLAGS = -Wall -Wextra
SOURCES = ircbot.cpp main.cpp BotFunctor.cpp
LIB = -lcurl -lws2_32
EXEC = IRCbot.exe
DEFINES = -DUW_CHUJNIA

all:
	$(CC) -O3 $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIB) $(DEFINES) -DVERBOSE

debug:
	$(CC) -g $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIB) $(DEFINES) -DVERBOSE -DSUPER_VERBOSE -DDEBUG

