#ifndef COMMANDS_H_
#define COMMANDS_H_

bool run_line(char *buffer);

// I KNOW THIS IS BAD PRACTICE BUT THERE IS NO TIME
extern pid_t CHILD_PIDS[4096];
extern int CHILD_PIDS_SIZE;

#endif // COMMANDS_H_