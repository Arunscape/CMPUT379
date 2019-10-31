#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../mapreduce.h"

void Map(char *file_name) {
  FILE *fp = fopen(file_name, "r");
  assert(fp != NULL);
  char *line = NULL;
  size_t size = 0;
  printf("Processing %s\n", file_name);
  while (getline(&line, &size, fp) != -1) {
    char *token, *dummy = line;
    while ((token = strsep(&dummy, " \t\n\r")) != NULL) {
      MR_Emit(token, "1");
      
    }
    
  }
  free(line);
  fclose(fp);
  printf("Finished processing %s\n", file_name);
}

void Reduce(char *key, int partition_number) {
  int count = 0;
  char *value, name[100];
  while ((value = MR_GetNext(key, partition_number)) != NULL)
    count++;
  sprintf(name, "result-%d.txt", partition_number);
  FILE *fp = fopen(name, "a");
  fprintf(fp, "%s: %d\n", key, count);
  fclose(fp);
  
}

int main(int argc, char *argv[]) {
  if (argc < 4){
    printf("Usage: ./distwc <num_mapper> <num_reducer> [files]... \n");
    return -1;
  }
  MR_Run(argc - 3, &(argv[3]), Map, atoi(argv[1]), Reduce, atoi(argv[2]));
}
