#include <vector>
#include <utility>
#include <algorithm>

#include <pthread.h>

#include "mapreduce.h"
#include "threadpool.h"
#include "reeeee.h"

struct partition;

//datastruct| partition|           key    value
// std::vector<std::vector<std::pair<char*, char*>>> shared_data;
std::vector<partition> shared_data;

struct partition {
  pthread_mutex_t mutex;
  //         KEY          VALUE
  std::vector<std::pair<const char*, const char*>> pairs;

  partition(){
    mutex = PTHREAD_MUTEX_INITIALIZER;
    pairs = std::vector<std::pair<const char*, const char*>>();
  }

};


void MR_Run(int num_files, char *filenames[],
            Mapper map, int num_mappers,
            Reducer concate, int num_reducers){
  
  ThreadPool_t* threadpool = ThreadPool_create(num_mappers);

  for (int i=0; i < num_files; i+=1){
    bool addedwork = ThreadPool_add_work(threadpool,(thread_func_t) map, filenames[i]);
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


bool compareByLength(const data &a, const data &b)
{
    return a.word.size() < b.word.size();
}

void MR_Emit(char *key, char *value){
  unsigned long partno = MR_Partition(key, num_partitions);
  std::pair<char*, char*> pair = std::make_pair(key, value);

  pthread_mutex_lock(&shared_data[partno]->mutex)
  shared_data[partno].insert(
        std::upper_bound(shared_date[partno].cbegin(), shared_data[partno].cend(), pair),
        pair
      );
  pthread_mutex_unlock(&shared_data[partno]->mutex)
}

void MR_ProcessPartition(int partition_number){
  ;
}

char *MR_GetNext(char *key, int partition_number){
  return 0;
}
