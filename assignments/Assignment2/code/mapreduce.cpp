#include <vector>
#include <utility>
#include <pthread.h>

#include "mapreduce.h"
#include "threadpool.h"
#include "reeeee.h"


//datastruct| partition|           key    value
std::vector<std::vector<std::pair<char*, char*>>> shared_data;

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

void MR_Emit(char *key, char *value){
  ;
}

unsigned long MR_Partition(char *key, int num_partitions){
  return 0;
}

void MR_ProcessPartition(int partition_number){
  ;
}

char *MR_GetNext(char *key, int partition_number){
  return 0;
}
