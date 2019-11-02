## How the intermediate key/value pairs are stored:
the intermediate key/pair values are shared in a vector names shared_data.
This is a global variable in mapreduce.cpp.

Its type is std::vector<partition>
, where partition is a custom struct which contains a multimap,
a pthread mutex, and some iterators used by the MR_GetNext function.

the multimap in partition is of type
std::multimap<char*, char*>
where the first char* is a key, and the second is a value.

Initially, partition contained an std::vector<std::pair<char*, char*>>
but I switched to a multimap thanks to the suggestion of a TA,
since it does the sorting automatically on insert and iterating over unique
keys was less painful. The switch was also pretty easy, since I already
had a custom compare function which uses strcmp, since comparing
char* pointers with < is a bad time...

So, effectively, I have a vector of multimaps,
where each multimap represents one partition. 


## Time complexity of MR_Emit
The time complexity of MR_Emit is the time complexity of inserting
into shared_data[partition_number]

under the hood, a multimap is a red black tree, and insertions have a
time complexity of O( log n )

## Time complexity of MR_GetNext
I use an iterator here, so the time complexity is O(n) since it gets
incremented until the key is found

## Data Structure used to implement task queue in threadpool
I used a priorityqueue in the threadpool to store the tasks.
Initially, I used a queue, and then I found out about the priorityqueue
during a lab, so that I no longer needed to call sort, it always starts
the task with the biggest file size first using my custom compare function
which uses the stat() syscall to get the file size

## Implementation of threadpool
well, we're implementing the functions in the .h file...

I guess I'll talk about what each function does.

As previously mentioned, there is a custom sort function for the threadpool
task queue, and the task queue itself is a priorityqueue. 

The threadpool_create function takes in a number, and creates a threadpool
with that many threads. It also initializes a mutex and a condition variable.


Threadpool destroy works by creating tasks as many as there are threads 
which have empty arguments.
Initially, I had them invoke a function that makes each thread exit itself, 
but I found I could simplify this by checking a boolean variable
named done in the main loop of the thread, then exiting from there. 

Each thread is
guaranteed to only pick up one of these since the thread will exit, and it will
no longer pick up a new task. 


Threadpool add work was pretty trivial to implement,
we create a task using the arguments provided,
acquire a pthread mutex lock,
push it to the task queue,
let sleeping threads waiting on the condition variable know that work is
available. True is returned when the task is added to the queue, 
and false is returned when the provided arguments are invalid. 

Get work was also pretty trivial to implement,
we acquire a pthread mutex lock,
see if the task queue has items in it, 
if so, return the task,
and if not, return NULL.
Of course, the mutex lock is unlocked before returning.

Thread run:
first of all, the signature of this function was modified from the original
header provided. It takes in a void* argument instead of Threadpool_t*
and will cast the void* argument into a threadpool pointer.
This was done to make the function compatible with pthread_create, which
requires the function signature to look like this. A wrapper function
could have been used here, but we were allowed to modify threadpool.h, so
I did so here. 

In the function, the thread will run in an infinite loop. It attempts to
get a task by calling threadpool get work. If the task is null, it will
wait on the condition variable, and if the task exists, it will run the task
and delete it from the queue. 

As the code is, the threads do not wait on the condition variable
and they instead stay in a busy for loop. 
My attempt at using the condition variable results in one
thread exiting, and the others stuck waiting on the condition variable.
Instead I just check if done is true.

However, this hack allows me to show that the
rest of the code works properly (I hope), that is, the running of the
user provided map and reduce functions. Commented out is my attempt to
use the condition variables and signalling instead  but I
could not get this working since I'm out of time and also sleep deprived.
I'm sure there is some trivial thing I'm missing that will be obvious when
I look at it later....
For the purposes of this assignment though, I always see the 

Congratulation, your mapreduce library is concurrency stable.

Message, and I've run this a LOT of times, even on death-incarnate.
It takes about 3-5 minutes to run each death test, and there are 3
in test.sh, meaning about 9-15 minutes to run the entire death-incarnate
test. This is significantly less than an hour.

## Synchronization primitives
pthread mutexes and 
pthread condition variables, 
were used for synchronization. 
Nothing else was used

for the threadpool, synchronization was trivial, we 
obtain a lock for the lines of code which modify the threadpool and unlock
when finished. 

I struggled a bit with the condition variables though (see above)
## Library functions and global variables
of course, the shared data structure is a global variable in mapreduce.h

the reducer function and number of reducers were made global,
otherwise there is no way of letting the MR_Emit call MR_Partition with
the correct number of partitions, or getting MR_ProcessPartition to
run the reducer function without changing the function signatures. 



## Testing correctness
Honestly, I wish I had the time to fully test everything, but my code
was mostly tested against the easy test cases and the death-incarnate test
cases provided in the eclass forum by the TA. 

I did get to visually inspect and verify that the mapping phase is working
correctly when my reducer wasn't working. I printed each key and the
partition it was going to , piped he output to sort, and verified that
identical words were going to the same partition. 

In my testing with the test cases provided on the forum,
I always get the message:
Congratulation, your mapreduce library is concurrency stable.

## Other comments
- the only leaks detected in my testing with valgrind were from other files
  using the mapreduce library. Ex: originating from diswc.cpp
- distwc.c was renamed to distwc.cpp and "1" was casted to (char*) 1 to make
  the file compatible with C++ 11

- reeeee.cpp: 
  I have a custom function named REEEEE, which ungracefully
  (i.e. it puts an error message in stderr, and calls exit(1)
  if there is user error in using the mapreduce library.)
  This was actually helpful while writing the code to see which parts
  of the program were not getting correct inputs. 
- the only leaks detected in my testing with valgrind were from other files
  using the mapreduce library. Ex: originating from diswc.cpp
  (i.e. user error in not freeiing their own memory)

# Misc. Sources
- https://stackoverflow.com/questions/9371236/is-there-an-iterator-across-unique-keys-in-a-stdmultimap
- https://en.cppreference.com/w/
- tldr.sh
- linux man pages v5
- thanks a lot to the lab TAs who helped solve some deadlock issues and
  catching weird bugs. 
- http://pages.cs.wisc.edu/~skrentny/cs367-common/readings/Red-Black-Trees/
- https://stackoverflow.com/questions/7576953/c-auto-keyword-why-is-it-magic
- https://stackoverflow.com/questions/35514909/how-to-clear-vector-in-c-from-memory
- https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/ptcinit.htm
