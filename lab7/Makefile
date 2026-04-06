CC=gcc
CFLAGS=-Wall -g

all: k0

k0: k0gram.tab.c lex.yy.c lex.yy.o k0gram.tab.o main.o tree.o token.o symtab.o syms.o type.o
	$(CC) $(CFLAGS) -o k0 k0gram.tab.o lex.yy.o main.o tree.o token.o symtab.o syms.o type.o

k0gram.tab.c k0gram.tab.h: k0gram.y
	bison -d -v k0gram.y

lex.yy.c: k0lex.l k0gram.tab.h tree.h
	flex k0lex.l

main.o: main.c tree.h symtab.h syms.h type.h
	$(CC) $(CFLAGS) -c main.c

tree.o: tree.c tree.h token.h type.h
	$(CC) $(CFLAGS) -c tree.c

token.o: token.c token.h
	$(CC) $(CFLAGS) -c token.c

symtab.o: symtab.c symtab.h type.h
	$(CC) $(CFLAGS) -c symtab.c

syms.o: syms.c syms.h tree.h token.h symtab.h type.h
	$(CC) $(CFLAGS) -c syms.c

type.o: type.c type.h tree.h symtab.h token.h k0gram.tab.h
	$(CC) $(CFLAGS) -c type.c

k0gram.tab.o: k0gram.tab.c
	$(CC) $(CFLAGS) -c k0gram.tab.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	rm -f k0 *.o k0gram.tab.c k0gram.tab.h lex.yy.c k0gram.output
