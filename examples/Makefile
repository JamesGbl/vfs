
CC = gcc
AR =ar

WARN=-g -Wall -fPIC -W -ansi -pedantic
INCS=-I./../include -I./../src
LIBS=-lpak -lm
BIGFILES=-D_FILE_OFFSET_BITS=64
CFLAGS = $(WARN) $(INCS) $(BIGFILES) 
LIB_OPTION=-shared

OBJS_TEST=test.o
OBJS_TEST2=test2.o

all:test test2

test:$(OBJS_TEST)
	$(CC) $(CFLAGS) -o ./../bin/test $(OBJS_TEST) $(LIBS) -L./../lib

objs/test.o:test.c
	$(CC) $(CFLAGS) -c test.c -o test.o 

test2:$(OBJS_TEST2)
	$(CC) $(CFLAGS) -o ./../bin/test2 $(OBJS_TEST2) $(LIBS) -L./../lib

objs/test2.o:test2.c
	$(CC) $(CFLAGS) -c test2.c -o test2.o 

clean:
	rm -f  ./../bin/test $(OBJS_TEST)
	rm -f  ./../bin/test2 $(OBJS_TEST2)
