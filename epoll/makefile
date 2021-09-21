CC = g++
OP = -std=c++11 -c

all: server

server: http.o server.o
	$(CC) -no-pie http.o server.o -o server -pthread -l sqlite3 -ljsoncpp

http.o: http.cpp
	$(CC)  $(OP) http.cpp

server.o: server.cpp
	$(CC)  $(OP) server.cpp

clean:
	rm *.o
