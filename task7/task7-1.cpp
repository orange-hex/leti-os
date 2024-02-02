/*
Работа 7.2 -- Взаимодействие процессов через очереди сообщений
Вариант с очередью стандарта SVID и функцией getrusage
Программа 1 -- Принимает сообщение из очереди
Митяков Д.А.
Группа 0324
*/

#include <bits/types/struct_rusage.h>
#include <cstring>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <unistd.h>

typedef struct {
    long mtype;
    char buff[256];
} TMessage;

int main(int argc, char* argv[]) {
    printf(":: main start ::\n");

    bool flag = true;
    char buffer;
    ssize_t bytes;
    struct rusage ru;

    // System V message queue identifier
    int msgid;
    // Message queue key
    key_t key = ftok(".", 'a');
    // Message to be received
    TMessage message;
    // Used to get queue statistics
    struct msqid_ds stats;

    // Nonblock stdin
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    // Open/create message queue
    msgid = msgget(key, 0);
    if (msgid < 0)
        msgid = msgget(key, IPC_CREAT | 0644);

    // Check queue statistics
    if (msgctl(msgid, IPC_STAT, &stats) == 0)
        printf("Messages in queue: %lu\nQueue size: %lu\n", stats.msg_qnum,
               stats.msg_qbytes);

    // Attempt to increase queue size
    stats.msg_qbytes = 17408;
    if (msgctl(msgid, IPC_SET, &stats) == 0)
        printf("Set queue size to %lu\n", stats.msg_qbytes);
    else {
        perror("msgctl");
        printf("Run program with superuser privileges or set cap_sys_resource "
               "capability using setcap.\n"
               "Example: sudo setcap cap_sys_resource=eip %s\n",
               argv[0]);
    }

    message.mtype = 1;

    while (flag) {
        // Clear message
        memset(message.buff, 0, sizeof message.buff);

        // Receive message
        if (msgrcv(msgid, &message, sizeof(message.buff), message.mtype,
                   IPC_NOWAIT) < 0)
            perror("msgrcv");
        else
            printf("%s", message.buff);

        bytes = read(STDIN_FILENO, &buffer, 1);
        if (bytes > 0) {
            printf("Received keypress...\n");
            flag = false;
        }

        sleep(1);
    }

    if (msgctl(msgid, IPC_STAT, &stats) == 0)
        printf("Messages in queue: %lu\nQueue size: %lu\n", stats.msg_qnum,
               stats.msg_qbytes);

    // Close message queue
    msgctl(msgid, IPC_RMID, NULL);

    printf(":: main exit ::\n");
    return 0;
}
