#include <bits/types/struct_rusage.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cstdlib>

int fd[2];

void* write_data(void* arg) {
    printf("write_data() start\n");

    int data;
    int* flag = (int*)arg;

    while (*flag != 1) {
        data = rand() % 10;
        if (write(fd[1], &data, sizeof(int)) != -1)
            printf("wrote %d\n", data);     // DELETE AFTER TESTING
        //sleep(1);
    }

    printf("write_data() exit\n");
    pthread_exit(NULL);
}

void* read_data(void* arg) {
    printf("read_data() start\n");

    int data;
    int* flag = (int*)arg;

    sleep(3);            // DELETE AFTER TESTING
    while (*flag != 1) {
        data = 0;
        read(fd[0], &data, sizeof(int));

        printf("\nread %d\n", data);

        //sleep(1);
    }

    printf("read_data() exit\n");
    pthread_exit(NULL);
}

int main() {
    printf("main() start\n");

    int flag1 = 0;
    int flag2 = 0;

    pthread_t proc1;
    pthread_t proc2;

    if (pipe(fd))
        return 1;

    pthread_create(&proc1, NULL, &write_data, &flag1);
    pthread_create(&proc2, NULL, &read_data, &flag2);

    printf("main() awaits keypress...\n");
    getchar();
    printf("main() received keypress\n");

    flag1 = 1;
    flag2 = 1;

    pthread_join(proc1, NULL);
    pthread_join(proc2, NULL);

    close(fd[0]);
    close(fd[1]);

    printf("main() exit\n");

    return 0;
}