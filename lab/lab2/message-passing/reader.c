#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct mesg_buffer { // structure for message queue
    long mesg_type;
    char mesg_text[100];
} message;

int main() {
    key_t key = ftok("progfile", 65); // generates a unique key
    int msgid = msgget(key, 0666 | IPC_CREAT); // gets the message queue with the unique key
    msgrcv(msgid, &message, sizeof(message), 1, 0); // receives the message
    printf("Data received is: %s \n", message.mesg_text);
    msgctl(msgid, IPC_RMID, NULL); // destroys the message queue
    return 0;
} // message-passing/reader.c