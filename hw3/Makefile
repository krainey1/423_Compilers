CC=gcc
CFLAGS=-Wall -g

all: parser

parser: k0gram.tab.c lex.yy.c main.o tree.o token.o
	$(CC) $(CFLAGS) -o parser k0gram.tab.c lex.yy.c main.o tree.o token.o -lfl

k0gram.tab.c k0gram.tab.h: k0gram.y
	bison -d -v k0gram.y

lex.yy.c: k0lex.l k0gram.tab.h
	flex k0lex.l

main.o: main.c tree.h
	$(CC) $(CFLAGS) -c main.c

tree.o: tree.c tree.h token.h
	$(CC) $(CFLAGS) -c tree.c

token.o: token.c token.h
	$(CC) $(CFLAGS) -c token.c

clean:
	rm -f parser *.o k0gram.tab.c k0gram.tab.h lex.yy.c k0gram.output

zip: clean
	zip -r hw3.zip . -x "*.zip" -x "k0gram.output" -x "lex.yy.c" -x "k0gram.tab.c" -x "k0gram.tab.h"