# DragonShell
by Arun Woosaree

awoosare

# Design choices
Dragonshell's prompt prints emoji, because they are cool and lots of modern terminal
emulators support them
🐲 dragonshell🐉 >

## Makefile
 - clang is used, because I confirmed with that TA that we can use it since clang
   is installed on the lab machines
 - dragonshell is not built with the debug flag as per my discussion with a TA
 - xxd is used to generate a .h file which contains the data needed to print the dragon. I did this because C does not have raw strings

## dragonshell.c
- houses the main loop of the program.
- a programmer should be able to see from a high level view how dragonshell works here  since most of the functions called explain the purpose in their names.
  e.g.: handle_signals(), add_to_path(), printDragon(), getline(), run_line(), cleanup_PATH()
- also contains logic to gracefully exit when EOF is sent by ^D

## array.c
- a custom array implementation
- used by dragonshell to store arrays of pointers to strings
- has a property .size, which was useful for determining the number of tokens in the
  array, without having to use strlen each time
- also allows for iterating over strings to make things feel a little more high level
- also cost me a lot of extra time, and was probably not worth the effort for this
  assignment, but it may be useful for future ones
- hindsight is always 2020 but this custom array implementation has made the
  code slightly more complicated

- a paths.c file was created. paths.h exposes:
	- add_to_path()
	- cleanup_PATH()
	- print_path()
	- char* get_absolute_path()

## commands.c
- the meat of dragonshell is here
- a recursive descent parser is used to first split by semicolons, pipes, redirects,
  and finally, spaces to run each command
- once parsed, we check for builtin commands, and if the token is not a builtin,
  we check directories in the user's $PATH variable that we store internally,
  (not the environment variable), the current directory, and the absolute path that
  the user may have entered
- for pipes and redirects, we create the file descriptors before running the program
  and pass them to the exec_from_path() function, which forks to get a child
  process, then dup2 's the file descriptors which were passed in. If the command
  was not a pipe or redirect, STDIN_FILENO and STDOUT_FILENO are passed in by
  default, and dup2 -ing them does not change the child process's stdin or stdout

## util.c
- houses the tokenizer function, which was modified to work with the custom
  array implementation in array.c
- also contains the signal handler which allows dragonshell (but not its forked 
  processes) to ignore signals sent by ^C, and ^Z

## welcome_dragon.c
- prints the DragonShell welcome message.


## Features

### Builtins
 
To implement the builtins, some if-else statements were used to check for the builtin
commands. After talking with TAs, these builtins do not fork()
They will not respond 

#### cd
- to implement cd, we first check that there is exactly one token after cd.
- if not, we let the user know about the syntax error
- if there is exactly one token, we pass that token into the *chdir()* system call
- if the system call fails, like for example an invalid path was entered, perror
  will notify the user

#### pwd
- to implement pwd, we first check that there are no tokens after pwd
- if not, we let the user know about the syntax error
- if it's good, then we use the *getcwd()* system call and print its result to stdout

#### a2path
- to implement a2path, we first check that there is exactly one argument after a2path
- we also do some rudimentary syntax checking. i.e. we check that the argument after
  starts with $PATH:
- the user will be warned if these syntax checks fail
- add_to_path() is then called, which takes in this argument sans the $PATH: at the
  beginning
- it was mentioned on the forum that a2path with no arguments should clear the path. this was also implemented

#### $PATH
- after checking the assignment spec and talking to a TA, this implementation of $PATH
  works if $PATH is the first token, and no other arguments are pased after it
- the user is warned if the syntax check fails
- if it passed that check, then print_path() is called from paths.c, which prints the
  PATH variable to stdout

#### exit
- to implement exit, we check if no other arguments exist.  
- the user is then notified is the syntax check fails
- if there is no syntax error, the chain of parsing functions returns boolean false
  which the main loop in the function checks for at run_line().
  Dragonshell will then attempt to gracefully exit

#### &
- if & is at the end of a command, we redirect its output to /dev/null 
- if there is an &, dragonshell assumes it is always at the end of a command
  as per the assignment spec and when confirming with TAs. 
  for example: sleep 10 &; cowsay moo will send sleep to the background but
  cowsay will still run immediately

### Running external programs

- external programs can be run if they are found in the $PATH variable intenally
  used by dragonshell (not to be confused with the environment variable as 
  per the assignment spec)
- multiple commands separated by ; work as well. I'm using strtok loops in a 
  form inspired by recursive decsent parsing. It's a primitive implementation
  which first separates by semicolons, &, |, > , and then spaces
- child processed run in the background are added to an array which keep track
  of their pids. I ran out of time during my implementation, otherwise I 
  would have used my custom array struct. I get it, global variables are not the best,
  and also some space is wasted on the stack. This array is of size 4096 of type
  pid_t, but old pids are overwritten, so it is very unlikely you'd hit the limit.
  In fact, I probably could have gone with something smaller in size (or just use
  my array implementation 😂🙄

#### piping
- piping works with external programs
- incidentally with my implementation, it seems to work with pwd as well,
  which is a built in command from my understanding this was not required, but
  it works!
- as per the assignment spec, dragonshell only supports one level of piping

#### redirection
- in my testing, redirection works as intended.
- the command after > is always assumed to be a file and so if it doesn't exist
  one is created with that name

#### signals
- dragonshell will ignore ^C and ^Z, but its child processes obey them
- dragonshell will gracefully quit when EOF is sent by pressing ^D

#### opening dragonshell within dragonshell
dragonshell can be run from within itself, however stdout can sometimes look weird like this:
🐲 dragonshell🐉 > 🐲 dragonshell🐉 > 
- this could be fixed by silencing stdout of the first instance and
  redirecting it when the second dragonshell is exited but this use case
  was not mentioned in the assignment spec so it was not done
- there might be an edge case with signal interrupts when dragonshell
  is run from itself where 🐲 dragonshell🐉 >  is printed continuously, 
  however an attempt was made to fix that and it does not seem to happen any more
  in my limited testing

## System calls
the versions available using man 2 were used. 
man pages version released with linux 5.02
- chdir()
- fork()
- execve()
- _exit()
- wait()
- wait_pid()
- open()
- close()
- dup2()
- pipe()
- kill()

## Testing
- aside from the assignment spec, we were not given many test cases
- I tried my best to come up with some cases myself given the limitations in 
  the assignment spec, and also clarified with TAs about certain things
- valgrind was used to make sure all memory was freed. In my testing,
  it always said no leaks are possible


# Credit
- the strok for loops I made were inspired by these while loops https://gist.github.com/efeciftci/9120921
- https://stackoverflow.com/questions/45079788/recursive-descent-parser-easy-to-get-explanation
- https://tldr.sh/
- https://www.archlinux.org/packages/core/any/man-pages/
- https://archive.archlinux.org/repos/2019/10/04/core/os/x86_64/man-pages-5.02-1-any.pkg.tar.xz
- Lab examples 1 for dup2 was inspiring
- Class example code for signal handling was also inspiring
- lab TAs helped out with pointers and mentioning useful functions

Example code from here was really helpful
- https://www.gnu.org/software/libc/manual/html_node/Line-Input.html
- https://www.gnu.org/software/libc/manual/html_node/Dynamic-Output.html



