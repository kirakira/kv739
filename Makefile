.PHONY:
	client server test all

FLAGS=-Wall -Wextra -O2

all: client server test

bin/common.o: src/common.h src/common.cc src/message.h
	mkdir -p bin/
	g++ -o bin/common.o ${FLAGS} -c src/common.cc

bin/common_pic.o: src/common.h src/common.cc src/message.h
	mkdir -p bin/
	g++ -o bin/common_pic.o ${FLAGS} -c -fPIC src/common.cc

bin/lib739kv.so: bin/common_pic.o src/client.h src/client.cc
	mkdir -p bin/
	g++ -o bin/client.o ${FLAGS} -c -fPIC src/client.cc
	g++ -shared -o bin/lib739kv.so bin/client.o bin/common_pic.o

bin/database.o: src/database.h src/database.cc src/message.h
	mkdir -p bin/
	g++ -o bin/database.o -c ${FLAGS} src/database.cc

bin/server: bin/common.o bin/database.o src/server.cc
	mkdir -p bin/
	g++ -o bin/server ${FLAGS} bin/common.o bin/database.o src/server.cc

bin/test: bin/lib739kv.so src/test.cc src/database.h src/message.h
	mkdir -p bin/
	g++ -o bin/test ${FLAGS} -Lbin/ -l739kv bin/common.o src/test.cc

client: bin/lib739kv.so

server: bin/server

test: bin/test
