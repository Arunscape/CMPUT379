// coarse grained lock - single global mutex
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 32
#define THREADS 16

int shared_array[ARRAY_SIZE];
pthread_mutex_t mutex[ARRAY_SIZE]; // one mutex per element

int replace(unsigned int index, int new_val) {
    int old_val;
    pthread_mutex_lock(&mutex[index]); // only lock the element
    old_val = shared_array[index];
    shared_array[index] = new_val;
    pthread_mutex_unlock(&mutex[index]);
    return old_val;
}

void* random_acccess(void * args) {
    // thread function
    unsigned seed = (unsigned) time(NULL); // rand() has a global lock. Use rand_r() instead.
    for (int i = 0; i < 1024000; i++) {
        replace(rand_r(&seed) % ARRAY_SIZE, rand_r(&seed)); // randomly replace an element
    }
    return NULL;
}

int main() {
    memset(shared_array, 0, sizeof(shared_array));
    for (int i = 0; i < ARRAY_SIZE; i ++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    pthread_t threads[THREADS];
    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, random_acccess, NULL);
    }
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}