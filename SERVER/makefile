debug = true
ifeq ($(debug), true)
CC = g++
OP = -std=c++17 -c -g -Debug -Wall -Wextra
else
CC = g++
OP = -std=c++17 -c -O2
endif

all: server

# 使用动态链接库： -L. <-l加去掉前缀lib和.so之后的名字>

server: server.o HttpServer.o GetProcessor.o PostProcessor.o UrlEncoder.o ProcessorFactory.o Redis.o libllhttp.so
	$(CC) -no-pie *.o -o server -pthread -ljsoncpp -L. -lllhttp -L/usr/local/lib/ -lhiredis
	rm *.o

server.o: server.cpp
	$(CC)  $(OP) server.cpp

HttpServer.o: HttpServer.cpp
	$(CC)  $(OP) HttpServer.cpp

GetProcessor.o: GetProcessor.cpp
	$(CC)  $(OP) GetProcessor.cpp

PostProcessor.o: PostProcessor.cpp
	$(CC)  $(OP) PostProcessor.cpp

UrlEncoder.o: UrlEncoder.cpp
	$(CC)  $(OP) UrlEncoder.cpp

ProcessorFactory.o: ProcessorFactory.cpp
	$(CC)  $(OP) ProcessorFactory.cpp

Redis.o: Redis.cpp
	$(CC)  $(OP) Redis.cpp

clean:
	rm *.o
