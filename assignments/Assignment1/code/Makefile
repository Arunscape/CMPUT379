CC = clang
CFLAGS = -Wall -g
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean test

all: dragonshell

clean:
	rm -f *.o dragonshell valgrind-out.txt welcome_dragon.h

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
 
dragonshell: welcome_dragon.h $(OBJECTS)
	$(CC) -o dragonshell $(OBJECTS)

test: leak
# default:
# 	clang -g -Wall dragonshell.c welcome.c commands.c -o dragonshell

leak: all
	valgrind --leak-check=full \
		 --show-leak-kinds=all \
		 --track-origins=yes \
		 --verbose \
		 ./dragonshell
		#  --log-file=valgrind-out.txt \

format:
	clang-format -i *.c *.h

welcome_dragon.h:
	xxd -i welcome_dragon > welcome_dragon.h
