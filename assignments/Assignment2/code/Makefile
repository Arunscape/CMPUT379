CC = g++ -pthread
CFLAGS = -Wall -Werror -O2
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

.PHONY: clean

# todo make default wc
.DEFAULT_GOAL := wc

clean:
	rm -f *.o wordcount valgrind-out.txt *.out

%.o: %.cpp
	${CC} ${CFLAGS} -c $< -o $@

#distwc.o: distwc.c
#	${CC} ${CFLAGS} -c -x c distwc.c -o distwc.o
 
compile: $(OBJECTS) #distwc.o
	$(CC) -o wordcount $(OBJECTS) #distwc.o

wc: # only link the files and make the executable
	$(CC) -o wordcount $(OBJECTS) #distwc.o

leak: compile
	valgrind --leak-check=full \
 		 --show-leak-kinds=all \
		 --track-origins=yes \
 		 --verbose \
 		 ./wordcount
	#  --log-file=valgrind-out.txt \

format:
	clang-format -i *.c *.h

#cptest: compile
#	cp -f reeeee.cpp reeeee.h mapreduce.cpp mapreduce.h threadpool.cpp threadpool.h ../testcases
# 
compress: 
	zip mapreduce.zip Makefile readme.md mapreduce.h mapreduce.cpp threadpool.h threadpool.cpp reeeee.h reeeee.cpp distwc.cpp
