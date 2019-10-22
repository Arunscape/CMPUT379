CC = clang++
CFLAGS = -Wall -Werror -pthread -g
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean

all: wc

clean:
	rm -f *.o dragonshell valgrind-out.txt *.out

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
 
compile: $(OBJECTS)
	$(CC) -o wordcount $(OBJECTS)

wc:
	$(CC) -o wordcount $(OBJECTS)

leak: all
	valgrind --leak-check=full \
 		 --show-leak-kinds=all \
		 --track-origins=yes \
 		 --verbose \
 		 ./dragonshell
	#  --log-file=valgrind-out.txt \

format:
	clang-format -i *.c *.h

