note: for read and write syscalls I used pread and pwrite

mount 
open syscall used to open the file

create
write syscall (need to update superblock)

delete
write syscall to update superblock and file

read
read syscall to read the block

write
write syscall to write the block

buffer
no syscalls

ls
no syscalls

resize
read and write syscall

defragment
read and write syscall

change directory
no syscall

design choices
early on I noticed that there was a lot of repetitive code.
where applicable I made re-usable functions. These are located in util.c
Things I had to do repeatedly like reading , writing to the file, getting 
an inode's parent, or size or used state, updating blocks, etc. 

I tested the program using the test cases provided, as well as some
test scripts which were tested visually by looking at the hex dump of the
resulting disk using xxd. These tests are in a tests/ folder

the rest was pretty straightforward, I had a separate process_input.c
for parsing commands, but the other fs_* functions remained in FileSystem.c
and I called the functions I re-used a lot which are located in util.c

credits:
gnu man pages
https://github.com/bminor/glibc/blob/5cb226d7e4e710939cff7288bf9970cb52ec0dfa/string/strtok_r.c#L73

I did a weird thing with strtok, which is very hacky but works with the glibc implementation. 

sort function used in defrag
https://en.cppreference.com/w/c/algorithm/qsort

lab TAs helped me a lot with the consistency checks, discussed the algorithms
for traversal with them. Also got inspiration for defragment function


valgrind
all heap blocks were freed in my testing. It throws warnings about me using
stcrpy in an unsafe way but I have ignored them. 
