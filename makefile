CC = g++
FLAGS = -Wall -Wextra
SRCS = main.cpp ircbot.cpp modules/www_stuff.cpp modules/print_wwwtitle.cpp modules/memo.cpp
LIB = -lcurl -L"c:\MinGW\bin" -lws2_32
EXEC = ircbot.exe

all:
	$(CC) $(FLAGS) -O3 $(SRCS) -o $(EXEC) $(LIB)
	
debug:
	$(CC) $(FLAGS) -g $(SRCS) -o $(EXEC) $(LIB)
