CC      = gcc
CFLAGS  = -Wall -Werror -std=c11 -g
TARGET  = tactest
SRCS    = tac.c main.c
OBJS    = $(SRCS:.c=.o)
 
.PHONY: all clean
 
all: $(TARGET)
 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
 
%.o: %.c tac.h
	$(CC) $(CFLAGS) -c -o $@ $<
 
clean:
	rm -f $(OBJS) $(TARGET)