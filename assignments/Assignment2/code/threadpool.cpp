#include <iostream>
#include "threadpool.h"
#include "reeeee.h"
/**
* A C style constructor for creating a new ThreadPool object
* Parameters:
*     num - The number of threads to create
* Return:
*     ThreadPool_t* - The pointer to the newly created ThreadPool object
*/
ThreadPool_t *ThreadPool_create(int num)
{

  if (num <= 0)
  {
    REEEEE("ERROR: ThreadPool_create num of threads entered was <=0");
  }
  // create thread
  ThreadPool_t *threadpool;
  if ((threadpool = (ThreadPool_t *)malloc(sizeof(ThreadPool_t))) == NULL)
  {
    REEEEE("ERROR: could not allocate space for the threadpool");
  }
  threadpool->num_threads = num;

  return threadpool;
}

/**
* A C style destructor to destroy a ThreadPool object
* Parameters:
*     tp - The pointer to the ThreadPool object to be destroyed
*/
void ThreadPool_destroy(ThreadPool_t *tp);

/**
* Add a task to the ThreadPool's task queue
* Parameters:
*     tp   - The ThreadPool object to add the task to
*     func - The function pointer that will be called in the thread
*     arg  - The arguments for the function
* Return:
*     true  - If successful
*     false - Otherwise
*/
bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg);

/**
* Get a task from the given ThreadPool object
* Parameters:
*     tp - The ThreadPool object being passed
* Return:
*     ThreadPool_work_t* - The next task to run
*/
ThreadPool_work_t *ThreadPool_get_work(ThreadPool_t *tp);

/**
* Run the next task from the task queue
* Parameters:
*     tp - The ThreadPool Object this thread belongs to
*/
void *Thread_run(ThreadPool_t *tp);
