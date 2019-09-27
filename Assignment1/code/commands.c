#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void do_commands(char** tokens){

    if (strcmp(tokens[0], "cd") == 0){
        chdir(tokens[1]);
    } 
    
    
    
    
    else {
        fork
    }
}

void attempt_evaluate_from_path(){
    __pid_t f = fork();

    if (f ){
        wait();
    } else {
        
    }
}