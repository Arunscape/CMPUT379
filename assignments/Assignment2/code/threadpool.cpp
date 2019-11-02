#include "threadpool.h"
#include "reeeee.h"
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void *Thread_run(void *tp);

bool LessThanByFileSize::operator()(const ThreadPool_work_t *t1,
                                    const ThreadPool_work_t *t2) const {
  if (t1 == NULL || t2 == NULL) {
    REEEEE("ERROR: One of the work items was null");
  }

  if (t1->func == NULL) {
    if (t2->func == NULL) {
      return false;
    }
    return true;
  }
  if (t2->func == NULL) {
    return false;
  }
  struct stat s1;
  struct stat s2;

  if (stat((const char *)t1->arg, &s1) == 0 &&
      stat((const char *)t2->arg, &s2) == 0) {
    return s1.st_size < s2.st_size;
  }
  perror("WARNING: stat()");
  //  REEEEE("ERROR could not read file size");
  return true;
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

  for (int i = 0; i < num; i += 1) {
    pthread_t thread;
    pthread_create(&thread, NULL, Thread_run, threadpool);
    threadpool->threads.push_back(thread);
  }

  threadpool->mutex = PTHREAD_MUTEX_INITIALIZER;
  if (pthread_mutex_init(&(threadpool->mutex), NULL) != 0) {
    REEEEE("ERROR: Could not init mutex");
  }

  threadpool->workavailable = PTHREAD_COND_INITIALIZER;
  if (pthread_cond_init(&(threadpool->workavailable), NULL) != 0) {
    REEEEE("ERROR: Could not init condition variable");
  }
  threadpool->done = false;

  return threadpool;
}

/**
 * A C style destructor to destroy a ThreadPool object
 * Parameters*:
 *     tp - The pointer to the ThreadPool object to be destroyed
 */
void ThreadPool_destroy(ThreadPool_t *tp) {

  pthread_mutex_lock(&(tp->mutex));
  tp->done = true;
  pthread_cond_broadcast(&(tp->workavailable));
  pthread_mutex_unlock(&(tp->mutex));

  for (auto &t : tp->threads) {
    (void)t;
    ThreadPool_add_work(tp, NULL, NULL);
    // std::cout<< "ADDED KILL TASK " << std::endl;
  }
  // wait for them to finish
  for (pthread_t &t : tp->threads) {
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
bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg) {

  if (tp == NULL) {
    return false;
  }

  ThreadPool_work_t *task = new ThreadPool_work_t();
  task->func = func;
  task->arg = arg;

  pthread_mutex_lock(&(tp->mutex));
  tp->tasks.work.push(task);
  // pthread_cond_signal(&(tp->workavailable));
  pthread_cond_broadcast(&(tp->workavailable));
  pthread_mutex_unlock(&(tp->mutex));

  return true;
};

/**
 * Get a task from the given ThreadPool object
 * Parameters:
 *     tp - The ThreadPool object being passed
 * Return:
 *     ThreadPool_work_t* - The next task to run
 */
ThreadPool_work_t *ThreadPool_get_work(ThreadPool_t *tp) {
  pthread_mutex_lock(&(tp->mutex));

  ThreadPool_work_t *t = NULL;
  if (tp->tasks.work.size() > 0) {
    t = tp->tasks.work.front();
    tp->tasks.work.pop();
  }
  pthread_mutex_unlock(&(tp->mutex));
  return t;
};

/**
 * Run the next task from the task queue
 * Parameters:
 *     tp - The ThreadPool Object this thread belongs to
 */
void *Thread_run(void *tp) {
  ThreadPool_t *threadpool = (ThreadPool_t *)tp;

  for (;;) {
    // get a task from the task queue and execute it
    //    std::cout << "I WANT WORK" << std::endl;
    ThreadPool_work_t *task = ThreadPool_get_work(threadpool);
    if (task != NULL) {
      if (task->func == NULL) {
        delete task;
        pthread_exit(NULL);
      }
      (task->func)(task->arg);
      delete task;
    } else {

      //      if (threadpool->done) {
      //        break;
      //      }

      pthread_mutex_lock(&(threadpool->mutex));

      while (!(threadpool->tasks.work.size() > 0) && !threadpool->done) {
        std::cout << "I WANT WORK" << std::endl;
        pthread_cond_wait(&(threadpool->workavailable), &(threadpool->mutex));
        std::cout << "I'M FREE" << std::endl;
      }

      pthread_mutex_unlock(&(threadpool->mutex));
    }
  }
  pthread_exit(NULL);
  return NULL;
}
