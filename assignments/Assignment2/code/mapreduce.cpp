#include <vector>
#include <utility>
#include <algorithm>

#include <pthread.h>

#include "mapreduce.h"
#include "threadpool.h"
#include "reeeee.h"

struct partition;
void* wrapper_function_because_we_cant_change_the_fucking_signature(void*);

//datastruct| partition|           key    value
// std::vector<std::vector<std::pair<char*, char*>>> shared_data;
std::vector<partition> shared_data;

// number of reducers
int R;

struct partition {
  pthread_mutex_t mutex;
  //         KEY          VALUE
  std::vector<std::pair<char*, char*>> pairs;

  partition(){
    mutex = PTHREAD_MUTEX_INITIALIZER;
    pairs = std::vector<std::pair<char*, char*>>();
  }

};


void MR_Run(int num_files, char *filenames[],
            Mapper map, int num_mappers,
            Reducer concate, int num_reducers){
  
  ThreadPool_t* threadpool = ThreadPool_create(num_mappers);

  if (num_reducers <= 0){
    REEEEE("You need at least one or more reducers!");
  }
  R = num_reducers;

  for (int i=0; i < num_files; i+=1){
    bool addedwork = ThreadPool_add_work(threadpool,(thread_func_t) concate, &shared_data[i]);
    if (!addedwork){
      REEEEE("Error adding work to threadpool");
    }
  }

  //if ( false ){ // k files are processed
  //  for(pthread_t &t: threadpool->threads){
  //    pthread_join(t, NULL);
  //  }
  //}

  // wait for threads to finish then destroy the pool
  ThreadPool_destroy(threadpool);


  // create R reducer threads
  ThreadPool_t* reducer_threadpool = ThreadPool_create(num_reducers);
  for(intptr_t i=0; i<= num_reducers; i+=1){
    bool addedwork = ThreadPool_add_work(reducer_threadpool,(thread_func_t) wrapper_function_because_we_cant_change_the_fucking_signature, (void*) i);
    if (!addedwork){
      REEEEE("Error adding work to threadpool");
    }
  }

}

// this function was copied from the assignment description
unsigned long MR_Partition(char* key, int num_partitions){
  unsigned long hash = 5381;
  int c;
  while ((c=*key++) != '\0')
    hash = hash * 33 + c;
  return hash % num_partitions;
}


void MR_Emit(char *key, char *value){

    // maps the key to an integer between 0 and R-1
  unsigned long partno = MR_Partition(key, R-1); 
  std::pair<char*, char*> pair = std::make_pair(key, value);

  // fine grained lock which locks only the partition being modified, and not the entire shared data structure
  pthread_mutex_lock(&shared_data[partno].mutex);
  // std::pair implements operator, so this should sort by key first then value in ascending order
  shared_data[partno].pairs.insert(
        std::upper_bound(shared_data[partno].pairs.cbegin(), shared_data[partno].pairs.cend(), pair),
        pair
      );
  pthread_mutex_unlock(&shared_data[partno].mutex);
}

// sorry, please excuse the name of this function
void* wrapper_function_because_we_cant_change_the_fucking_signature(void* p){
  int partno = (intptr_t) p;

  MR_ProcessPartition(partno);
  return NULL;
}
void MR_ProcessPartition(int partition_number){
 // run the user defined Reduce function
 // on the next unprocessed key in the given partition in a loop
 // (Reduce is only invoked once per key)
  for (;;){
    // char* next_key = MR_GetNext(what, partition_number);
  }
}

char *MR_GetNext(char *key, int partition_number){
  // called by the user provided Reducer function 
  // returns the next value associated with a given key in the sorted partition
  // returns NULL when the keys's values have been sorted correctly
  return NULL;
}

