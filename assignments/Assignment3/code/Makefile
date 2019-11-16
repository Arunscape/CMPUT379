CC = clang
CFLAGS = -Wall
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean test

all: fs

clean:
	rm -f *.o  *.out
	rm -f disk0
	./create_fs disk0

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
 
fs: $(OBJECTS)
	$(CC) -o fs $(OBJECTS)


# add -g to CC flags before enabling
# leak: all
# 	valgrind --leak-check=full \
# 		 --show-leak-kinds=all \
# 		 --track-origins=yes \
# 		 --verbose \
# 		 ./dragonshell
# 		#  --log-file=valgrind-out.txt \

format:
	clang-format -i *.c *.h