.PHONY:
	client all

all: client

bin/common.o: src/common.h src/common.cc src/message.h
	mkdir -p bin/
	g++ -o bin/common.o -Wall -Wextra -c src/common.cc

bin/lib739kv.so: bin/common.o src/client.h src/client.cc
	mkdir -p bin/
	g++ -o bin/client.o -Wall -Wextra -c -fpic src/client.cc
	g++ -shared -o bin/lib739kv.so bin/client.o bin/common.o

client: bin/lib739kv.so
