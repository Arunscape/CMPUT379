#include "threadpool.h"
#include "reeeee.h"

void *Thread_run(void *tp);

/**
 * A C style constructor for creating a new ThreadPool object
 * Parameters:
 *     num - The number of threads to create
 * Return:
 *     ThreadPool_t* - The pointer to the newly created ThreadPool object
 */
ThreadPool_t *ThreadPool_create(int num) {

  if (num <= 0) {
    REEEEE("ERROR: ThreadPool_create num of threads entered was <=0");
  }
  // create thread
  ThreadPool_t *threadpool = new ThreadPool_t();

  for (int i=0; i<=num; i+=1){
    pthread_t thread;
    pthread_create(&thread, NULL, Thread_run, threadpool);
//    threadpool->threads.push_back(thread);
//    the threads maybe don't even need to be stored.
  }

  threadpool->mutex = PTHREAD_MUTEX_INITIALIZER;
  if(pthread_mutex_init(&(threadpool->mutex), NULL) != 0){
        REEEEE("ERROR: Could not init mutex");
  }

  threadpool->workavailable = PTHREAD_COND_INITIALIZER

  return threadpool;
}

/**
 * A C style destructor to destroy a ThreadPool object
 * Parameters*:
 *     tp - The pointer to the ThreadPool object to be destroyed
 */
void ThreadPool_destroy(ThreadPool_t *tp){
  delete tp;
  pthread_mutex_destroy(&(tp->mutex));
  // todo delete queue of pointers
};

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
bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg){
  
  ThreadPool_work_t* task = new ThreadPool_work_t();
  task->func = func;
  task->arg = arg;
  
  pthread_mutex_lock(&(tp->mutex));
  tp->tasks.work.push(task);
  pthread_cond_signal(&(tp->workavailable));
  pthread_mutex_unlock(&(tp->mutex));

  return true;

  // TODO if error, return false
};

/**
 * Get a task from the given ThreadPool object
 * Parameters:
 *     tp - The ThreadPool object being passed
 * Return:
 *     ThreadPool_work_t* - The next task to run
 */
ThreadPool_work_t* ThreadPool_get_work(ThreadPool_t *tp){
    pthread_mutex_lock(&(tp->mutex));

    while(tp->tasks.work.size() <= 0){
      pthread_cond_wait(&(tp->workavailable), &(tp->mutex));
    }
    
    // I could get rid of this if statement
    if (tp->tasks.work.size() > 0){
      ThreadPool_work_t* t =  tp->tasks.work.front();
      tp->tasks.work.pop();
      return t;
      pthread_mutex_unlock(&(tp->mutex));
    }
    return NULL;
};

/**
 * Run the next task from the task queue
 * Parameters:
 *     tp - The ThreadPool Object this thread belongs to
 */
void *Thread_run(void *tp){
  ThreadPool_t* threadpool = (ThreadPool_t*) tp;

  for (;;){
     // get a task from the task queue and execute it
     ThreadPool_work_t* task = ThreadPool_get_work(threadpool);
     if (task != NULL){
        (task->func)(task->arg);
     }
  }
  pthread_exit(NULL);
  return NULL;
}
