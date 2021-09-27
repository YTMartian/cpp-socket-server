CC = g++
OP = -std=c++17 -c -O2 -Wall -Wextra

all: server

server: server.o HttpServer.o GetProcessor.o PostProcessor.o libllhttp.so
	$(CC) -no-pie server.o HttpServer.o GetProcessor.o PostProcessor.o -o server -pthread -l sqlite3 -ljsoncpp
	rm *.o

server.o: server.cpp
	$(CC)  $(OP) server.cpp

HttpServer.o: HttpServer.cpp
	$(CC)  $(OP) HttpServer.cpp

GetProcessor.o: GetProcessor.cpp
	$(CC)  $(OP) GetProcessor.cpp

PostProcessor.o: PostProcessor.cpp
	$(CC)  $(OP) PostProcessor.cpp
clean:
	rm *.o
