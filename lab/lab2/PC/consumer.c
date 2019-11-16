#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>

int main() { 
  const int SIZE = 4096;                   /* size (B) of shared memory object */
  const char* name = "/PC";                /* name of the shared memory object */
  
  int shm_fd;                              /* shared memory file descriptor */
  void* ptr;                               /* pointer to shared memory object */

  shm_fd = shm_open(name, O_RDONLY, 0666); /* open the shared memory object */
  
  ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0); /* memory map the shared memory object */

  printf("%s", (char*)ptr);                /* read from the shared memory object */
  shm_unlink(name);                        /* remove the shared memory object */
  
  return 0; 
}