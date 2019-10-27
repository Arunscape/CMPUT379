#include "mapreduce.h"

void MR_Run(int num_files, char *filenames[],
            Mapper map, int num_mappers,
            Reducer concate, int num_reducers){
  ;
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
