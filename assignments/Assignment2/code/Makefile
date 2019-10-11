CC = clang
CFLAGS = -Wall -Werror -pthread -g
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean test

all: dragonshell

clean:
	rm -f *.o dragonshell valgrind-out.txt *.out

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
 
dragonshell: $(OBJECTS)
	$(CC) -o dragonshell $(OBJECTS)


leak: all
	valgrind --leak-check=full \
 		 --show-leak-kinds=all \
		 --track-origins=yes \
 		 --verbose \
 		 ./dragonshell
	#  --log-file=valgrind-out.txt \

format:
	clang-format -i *.c *.h

