#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct mesg_buffer { // structure for message queue
    long mesg_type;
    char mesg_text[100];
} message;

int main() {
    key_t key = ftok("progfile", 65); // generates a unique key
    int msgid = msgget(key, 0666 | IPC_CREAT); // creates a message queue
    message.mesg_type = 1; // specify a message type
    printf("Write Data: ");
    scanf("%s", message.mesg_text); // read text from stdin to the message body
    msgsnd(msgid, &message, sizeof(message), 0); // sends the message
    printf("Data sent is: %s \n", message.mesg_text);
    return 0;
} // message-passing/writer.c