# makefile for macosx
# Improved GCC flags:
# -g:      adds debugging stuff for use with gdb
# -O0:     turns off optimization
# -Wall:   all warnings
# -Wextra: even more warnings. Can be annoying or helpful.
# -std=gnu99: uses a reasonable version of C
# -lm:     links the math library which is stupid, used in kvs.c


flags = -g -O0 -Wall -Wextra -std=gnu99 -lm

% : %.c 
	gcc $< -o $@ ${flags}

all : Server Client hash

hash : kvs2

server : Server
	./Server

client : Client
	./Client

clean:
	@rm -f Server Client kvs *~
	@rm -rf *.dSYM

spotless: clean
	@rm -f example.store hashtable
