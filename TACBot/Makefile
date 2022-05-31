CC=g++
CFLAGS=-std=c++11

TARGETS=TACbot.exe

build:	$(TARGETS)

TACbot.exe:	MyBot.cpp
		$(CC) $(CFLAGS) MyBot.cpp -o TACbot.exe
	
run:
	./TACbot.exe

clean:
	rm -f $(TARGETS)