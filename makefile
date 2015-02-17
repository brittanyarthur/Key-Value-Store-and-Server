# makefile for macosx

flags = -Wall # -O3

% : %.c 
	gcc $< -o $@ ${flags}

all : Server Client

server : Server
	./Server

client : Client
	./Client

clean:
	rm -f Server Client

