CC=gcc
CFLAGS=-Wall -g

all: parser

parser: parser.tab.c lex.yy.c main.o tree.o token.o
	$(CC) $(CFLAGS) -o parser parser.tab.c lex.yy.c main.o tree.o token.o -lfl

parser.tab.c parser.tab.h: parser.y
	bison -d -v parser.y

lex.yy.c: k0lex.l parser.tab.h
	flex k0lex.l

main.o: main.c tree.h
	$(CC) $(CFLAGS) -c main.c

tree.o: tree.c tree.h token.h
	$(CC) $(CFLAGS) -c tree.c

token.o: token.c token.h
	$(CC) $(CFLAGS) -c token.c

clean:
	rm -f parser *.o parser.tab.c parser.tab.h lex.yy.c parser.output

zip: clean
	zip -r hw3.zip . -x "*.zip" -x "parser.output" -x "lex.yy.c" -x "parser.tab.c" -x "parser.tab.h"