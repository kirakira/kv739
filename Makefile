.PHONY:
	client server test all

all: client server test

bin/common.o: src/common.h src/common.cc src/message.h
	mkdir -p bin/
	g++ -o bin/common.o -Wall -Wextra -c src/common.cc

bin/lib739kv.so: bin/common.o src/client.h src/client.cc
	mkdir -p bin/
	g++ -o bin/client.o -Wall -Wextra -c -fpic src/client.cc
	g++ -shared -o bin/lib739kv.so bin/client.o bin/common.o

bin/database.o: src/database.h src/database.cc src/message.h
	mkdir -p bin/
	g++ -o bin/database.o -c src/database.cc

bin/server: bin/common.o bin/database.o src/server.cc
	mkdir -p bin/
	g++ -o bin/server -Wall -Wextra bin/common.o bin/database.o src/server.cc

bin/test: bin/lib739kv.so src/test.cc
	mkdir -p bin/
	g++ -o bin/test -Wall -Wextra -Lbin/ -l739kv bin/common.o src/test.cc

client: bin/lib739kv.so

server: bin/server

test: bin/test
