CC = clang++ -pthread
CFLAGS = -Wall -Werror -g
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

.PHONY: clean

# todo make default wc
.DEFAULT_GOAL := compile

clean:
	rm -f *.o wordcount valgrind-out.txt *.out

%.o: %.cpp
	${CC} ${CFLAGS} -c $< -o $@

#distwc.o: distwc.c
#	${CC} ${CFLAGS} -c -x c distwc.c -o distwc.o
 
compile: $(OBJECTS) #distwc.o
	$(CC) -o wordcount $(OBJECTS) #distwc.o

wc:
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

cptest: compile
	cp -f reeeee.cpp reeeee.h mapreduce.cpp mapreduce.h threadpool.cpp threadpool.h ../testcases

#todo 
# compress: 
