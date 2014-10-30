
CC=gcc
INCLUDE=include/
CFLAGS = -g   -Wall  -I$(INCLUDE)
LIBS=-lpthread -Llib/ -lmylib
PROGS=library pc
DEBUG=

all: $(PROGS)

library:
	cd libsrc; make install

pc: pc.o  Item.o Item.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ pc.o Item.o  $(LIBS)

clean:
	cd libsrc; make installclean
	/bin/rm -f *.o pc
