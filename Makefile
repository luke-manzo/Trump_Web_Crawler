CC=gcc
CFLAGS=-c -Wall -g 

all: trump_crawler

trump_crawler: web.o trump_crawler.o
	$(CC) web.o trump_crawler.o -o trump_crawler -lpthread -lcurl

web.o: web.c
	$(CC) $(CFLAGS) web.c

trump_crawler.o: trump_crawler.c
	$(CC) $(CFLAGS) trump_crawler.c

clean:
	/bin/rm -f trump_crawler *.o *.gz

run:
	./trump_crawler url.txt

tarball:
	tar -cvzf manzo.tar.gz *

