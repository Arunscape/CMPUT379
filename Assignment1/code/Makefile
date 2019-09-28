CC = clang
CFLAGS = -Wall -g
SOURCES = $(wildcard *.c) welcome_dragon.c
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean test

all: dragonshell

clean:
	rm *.o dragonshell valgrind-out.txt

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
 
dragonshell: $(OBJECTS)
	$(CC) -o dragonshell $(OBJECTS)

test: leak
# default:
# 	clang -g -Wall dragonshell.c welcome.c commands.c -o dragonshell

leak: all
	valgrind --leak-check=full \
		 --show-leak-kinds=all \
		 --track-origins=yes \
		 --verbose \
		 --log-file=valgrind-out.txt \
		 ./dragonshell

format:
	clang-format -i *.c

#welcome_dragon.c:
#	xxd -i welcome_dragon
