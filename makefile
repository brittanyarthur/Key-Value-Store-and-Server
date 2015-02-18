# makefile for macosx
# Improved GCC flags:
# -g:      adds debugging stuff for use with gdb
# -O0:     turns off optimization
# -Wall:   all warnings
# -Wextra: even more warnings. Can be annoying or helpful.
# -std=gnu99: uses a reasonable version of C

flags = -g -O0 -Wall -Wextra -std=gnu99

% : %.c 
	gcc $< -o $@ ${flags}

all : Server Client

kvs: kvs.c

server : Server
	./Server

client : Client
	./Client

clean:
	@rm -f Server Client
