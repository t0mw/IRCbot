CC = g++
CFLAGS = -Wall -Wextra
SOURCES = ircbot.cpp main.cpp modules/www_stuff.cpp modules/print_wwwtitle.cpp modules/memo.cpp modules/sjp_pwn_info.cpp
LIB = -lcurl
EXEC = IRCbot_qrw
DEFINES = -DUW_CHUJNIA

all:
	$(CC) -O3 $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIB) $(DEFINES)

debug:
	$(CC) -g $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIB) $(DEFINES) -DVERBOSE -DSUPER_VERBOSE -DDEBUG

