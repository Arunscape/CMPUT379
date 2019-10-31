#include "threadpool.h"
#include "reeeee.h"
#include <sys/stat.h>

void *Thread_run(void *tp);

bool LessThanByFileSize::operator()(const ThreadPool_work_t* t1, const ThreadPool_work_t* t2) const {
    if (t1 == NULL || t2 == NULL){
      REEEEE("ERROR: One of the work items was null");
}

    struct stat s1;
    struct stat s2;

    if (stat((const char*)t1->arg, &s1) == 0 && stat((const char*)t2->arg, &s2)){
      return s1.st_size < s2.st_size;
    }
    REEEEE("ERROR could not read file size");
    return false; // REE will sys.exit(1) so this is to supporess a clang warning
};

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
    threadpool->threads.push_back(thread);
  }

  threadpool->mutex = PTHREAD_MUTEX_INITIALIZER;
  if(pthread_mutex_init(&(threadpool->mutex), NULL) != 0){
        REEEEE("ERROR: Could not init mutex");
  }

  threadpool->workavailable = PTHREAD_COND_INITIALIZER;
  
  return threadpool;
}

/**
 * A C style destructor to destroy a ThreadPool object
 * Parameters*:
 *     tp - The pointer to the ThreadPool object to be destroyed
 */
void ThreadPool_destroy(ThreadPool_t *tp){
  
  // ?? somehow signal to them that the threads need to exit. 
  pthread_mutex_lock(&(tp->mutex));
  tp->done = true;
  pthread_cond_signal(&(tp->workavailable));
  pthread_mutex_unlock(&(tp->mutex));
  
  // wait for them to finish
  for(pthread_t &t: tp->threads){
    pthread_join(t, NULL);
  }


  // delete it
  pthread_mutex_destroy(&(tp->mutex));
  delete tp;
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

    while(!(tp->tasks.work.size() > 0)){
      if(tp->done){
        pthread_exit(NULL);
      }
      pthread_cond_wait(&(tp->workavailable), &(tp->mutex));
    }
    
    ThreadPool_work_t* t =  tp->tasks.work.top();
    tp->tasks.work.pop();
    pthread_mutex_unlock(&(tp->mutex));
    return t;
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
     (task->func)(task->arg);
     delete task;
  }
  pthread_exit(NULL);
  return NULL;
}
