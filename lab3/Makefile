CC      := gcc
CFLAGS  := -g -Wall
LEX     := flex
YACC    := bison

TARGET  := ns

YFILE   := k0gram.y
LFILE   := k0lex.l
MAIN    := main.c

Y_C     := k0gram.tab.c
Y_H     := k0gram.tab.h
L_C     := k0lex.yy.c

OBJS    := k0gram.tab.o k0lex.yy.o main.o

all: $(TARGET)

# bison & flex stuff 
$(Y_C) $(Y_H): $(YFILE)
	$(YACC) -d -o $(Y_C) $(YFILE)

$(L_C): $(LFILE) $(Y_H)
	$(LEX) -o $(L_C) $(LFILE)


k0gram.tab.o: $(Y_C)
	$(CC) $(CFLAGS) -c $(Y_C) -o $@

k0lex.yy.o: $(L_C)
	$(CC) $(CFLAGS) -c $(L_C) -o $@

main.o: $(MAIN)
	$(CC) $(CFLAGS) -c $(MAIN) -o $@


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lfl -o $(TARGET)


clean:
	rm -f $(TARGET) *.o $(Y_C) $(Y_H) $(L_C)
