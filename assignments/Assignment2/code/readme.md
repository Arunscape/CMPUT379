## How the intermediate key/value pairs are stored:
the intermediate key/pair values are shared in a vector names shared_data.
This is a global variable in mapreduce.cpp.

Its type is std::vector<partition>
, where partition is a custom struct which contains a multimap,
pthread mutex, and some iterators used by the MR_GetNext function.

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
which invoke a function that makes each thread exit itself. Each thread is
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


Now, to address the sleep. Without it, the code mostly works, but then
my method for exiting the threads when all the tasks are done don't seem to
work. That is, using the test cases provided by the TA in the forum,
I will see that the files are done processing, but then one thread exits,
while the rest are stuck waiting on the condition variable workavailable.
The sleep resolves this issue, but I know it is not reliable, and it is
not the best way to do it. Also, with no doubt it increases the running time
of the code, since CPU cycles are wasted. I'm sure without it, my code would
work in seconds on the death-incarnate test case instead of about 5 minutes.
Still, 5 minutes is way less than an hour.
(That is, 5 minutes for each test case approximately.
so, for the death-incarnate, there are 3 invocations of distwc, which
in total take 5 + 5 + 5 = about 15 minutes)

However, this hack allows me to show that the
rest of the code works properly (I hope), that is, the running of the
user provided map and reduce functions. Commented out is my attempt to
use the condition variables and signalling instead of the sleep, but I
could not get this working since I'm out of time and also sleep deprived.
I'm sure there is some trivial thing I'm missing that will be obvious when
I look at it later....
For the purposes of this assignment though, I always see the 

Congratulation, your mapreduce library is concurrency stable.

Message, and I've run this a LOT of times. 
## Synchronization primitives
pthread mutexes and 
pthread condition variables, 
were used for synchronization. 
Nothing else was used


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
partition it was going to piped he output to sort, and verified that
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

