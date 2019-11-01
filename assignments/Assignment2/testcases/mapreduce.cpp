#include <algorithm>
#include <map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <iostream>

#include <pthread.h>
#include <string.h>

#include "mapreduce.h"
#include "reeeee.h"
#include "threadpool.h"

struct partition;
void *wrapper_function_because_we_cant_change_the_fucking_signature(void *);
Reducer reducer_function;

// datastruct| partition|           key    value
// std::vector<std::vector<std::pair<char*, char*>>> shared_data;
std::vector<partition> shared_data = std::vector<partition>();

std::vector<char*> char_garbage = std::vector<char*>();

// number of reducers
int R;

struct CharCompare {
  bool operator()(char *left, char *right) { return strcmp(left, right) < 0; }
};

struct partition {
  pthread_mutex_t mutex;
  //            KEY   VALUE
  std::multimap<char *, char *, CharCompare> pairs;
  //  std::multimap<char* , char*>::iterator reducer_iterator;

  std::multimap<char*, char*, CharCompare>::iterator it_first;
  std::multimap<char*, char*, CharCompare>::iterator it_end;

  partition() {
    mutex = PTHREAD_MUTEX_INITIALIZER;
    pairs = std::multimap<char *, char *, CharCompare>();
    //    reducer_iterator = std::multimap<char*, char*,
    //    CharCompare>::iterator();
  }
};

void MR_Run(int num_files, char *filenames[], Mapper map, int num_mappers,
            Reducer concate, int num_reducers) {

  ThreadPool_t *threadpool = ThreadPool_create(num_mappers);

  if (num_reducers <= 0) {
    REEEEE("ERROR: You need at least one or more reducers!");
  }

  if (num_files <= 0) {
    REEEEE("ERROR: Need to process at least one file!");
  }

  R = num_reducers;
  reducer_function = concate;
  for (int i = 0; i < num_reducers; i += 1) {
    partition *p = new partition();
    shared_data.push_back(*p);
  }

  for (int i = 0; i < num_files; i += 1) {
    bool addedwork =
        ThreadPool_add_work(threadpool, (thread_func_t)map, filenames[i]);
    if (!addedwork) {
      REEEEE("Error adding work to threadpool");
    }
  }

  ThreadPool_destroy(threadpool);

  // create R reducer threads
  //
  // I just realized threadpool is very specialized for mapping, since
  // the priorityqueue sorts based on file size, and there are no files here
  // so, I can't use the same threadpool for the reduce stage
  std::vector<pthread_t> reducer_threads;
  for (intptr_t i = 0; i < num_reducers; i += 1) {
    pthread_t thread;
    pthread_create(
        &thread, NULL,
        wrapper_function_because_we_cant_change_the_fucking_signature,
        (void *)i);
    reducer_threads.push_back(thread);
  }

  for (auto &t : reducer_threads) {
    pthread_join(t, NULL);
  }
  // done

//  for (auto &c : char_garbage){
//    free(c);
//  }
}

// this function was copied from the assignment description
unsigned long MR_Partition(char *key, int num_partitions) {
  unsigned long hash = 5381;
  int c;
  while ((c = *key++) != '\0')
    hash = hash * 33 + c;
  return hash % num_partitions;
}

void MR_Emit(char *key, char *value) {

  // maps the key to an integer between 0 and R-1
  unsigned long partno = MR_Partition(key, R);

  // fine grained lock which locks only the partition being modified, and not
  char* k = strdup(key); // todo garbage collect
  char* v = strdup(value);

  //char_garbage.push_back(k);
  //char_garbage.push_back(v);

  // the entire shared data structure
  pthread_mutex_lock(&shared_data[partno].mutex);
  shared_data[partno].pairs.insert(std::make_pair(k, v));
  pthread_mutex_unlock(&shared_data[partno].mutex);
}

// sorry, please excuse the name of this function
void *wrapper_function_because_we_cant_change_the_fucking_signature(void *p) {
  int partno = (intptr_t)p;
  
  MR_ProcessPartition(partno);
  pthread_exit(NULL);
  return NULL;
}
void MR_ProcessPartition(int partition_number) {
  // run the user defined Reduce function
  // on the next unprocessed key in the given partition in a loop
  // (Reduce is only invoked once per key)

  //  https://stackoverflow.com/questions/9371236/is-there-an-iterator-across-unique-keys-in-a-stdmultimap
  const auto compareFirst = [](const std::pair<char *, char *> &lhs,
                               const std::pair<char *, char *> &rhs) {
    return strcmp(lhs.first, rhs.first) < 0;
  };

// for (auto it=shared_data[partition_number].pairs.begin();
//      it != shared_data[partition_number].pairs.end(); ++it){
//    std::cout<< it->first << partition_number <<  std::endl;
// }


  for (auto it = shared_data[partition_number].pairs.begin();
       it != shared_data[partition_number].pairs.end();
       it = std::upper_bound(it, shared_data[partition_number].pairs.end(), *it,
                             compareFirst)) {
  //  std::cout<< it->first << partition_number <<  std::endl;

    char* key = it->first;

    auto iter_pair = shared_data[partition_number].pairs.equal_range(key);
    shared_data[partition_number].it_first = iter_pair.first;
    shared_data[partition_number].it_end = iter_pair.second;
  
    reducer_function(key, partition_number);
  }
}

char *MR_GetNext(char *key, int partition_number) {
  // called by the user provided Reducer function
  // returns the next value associated with a given key in the sorted
  // partition returns NULL when the keys's values have been sorted correctly

  if (shared_data[partition_number].it_first == shared_data[partition_number].it_end) {
    return NULL;
  }

  char *value = strdup(shared_data[partition_number].it_first->second);
  ++shared_data[partition_number].it_first;
  return value;
}
