CC = clang
CFLAGS = -Wall
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean test

all: fs

clean:
	rm -f *.o  *.out vgcore.*
	rm -f fs

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
 
fs: $(OBJECTS)
	$(CC) -o fs $(OBJECTS)

compile: $(OBJECTS)

 # add -g to CC flags before enabling
#leak: all
#	valgrind --leak-check=full \
#		 --show-leak-kinds=all \
#		 --track-origins=yes \
#		 --verbose \
#		 ./fs trivial-input
# 		#  --log-file=valgrind-out.txt \

format:
	clang-format -i *.c *.h

compress:
	zip fs-sim.zip Makefile readme.md *.h *.c tests/
