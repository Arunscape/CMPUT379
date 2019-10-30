#include "mapreduce.h"
#include "threadpool.h"
#include "reeeee.h"

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
