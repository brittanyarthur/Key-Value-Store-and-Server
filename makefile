# makefile for macosx
# Improved GCC flags:
# -g:      adds debugging stuff for use with gdb
# -O0:     turns off optimization
# -Wall:   all warnings
# -Wextra: even more warnings. Can be annoying or helpful.
# -std=gnu99: uses a reasonable version of C
# -lm:     links the math library which is stupid, used in kvs.c


flags = -g -O0 -Wall -Wextra -std=gnu99 -lm
OBJECTS = kvs2.o testing.o

% : %.c 
	gcc $< -o $@ ${flags}

all : Server Client testing

server : Server
	./Server

client : Client
	./Client

testing : $(OBJECTS)
	gcc $(OBJECTS) -o testing

kvs2.o : kvs2.c
	gcc -c kvs2.c

testing.o : testing.c
	gcc -c testing.c

clean:
	@rm -f Server Client kvs testing *~
	@rm -rf *.dSYM
	@rm -f kvs2.o testing.o

spotless: clean
	@rm -f example.store hashtable
