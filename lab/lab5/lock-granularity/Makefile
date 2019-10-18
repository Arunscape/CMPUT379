.PHONY: clean

all: fine coarse

fine: concurrent_array_fine.c
	gcc -o fine concurrent_array_fine.c -Wall -pthread -O3

coarse: concurrent_array.c
	gcc -o coarse concurrent_array.c -Wall -pthread -O3

clean:
	rm ./fine ./coarse
