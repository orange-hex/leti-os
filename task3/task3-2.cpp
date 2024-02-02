/*
Работа 3 -- Взаимодействие потоков через неименованные каналы
Вариант 3.1
Задание 2 -- Снятие блокировки с использованием pipe2
Митяков Д.А.
Группа 0324
*/

#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

int fd[2];

void* write_data(void* arg) {
    printf("write_data() start\n");

    struct rusage ru;
    int* flag = (int*)arg;

    while (*flag != 1) {
        if (getrusage(RUSAGE_SELF, &ru) == 0)
            if (write(fd[1], &ru, sizeof(struct rusage)) == -1)
                perror("write_data");

        sleep(1);
    }

    printf("write_data() exit\n");
    pthread_exit(NULL);
}

void* read_data(void* arg) {
    printf("read_data() start\n");

    struct rusage ru;
    int* flag = (int*)arg;

    while (*flag != 1) {
        if (read(fd[0], &ru, sizeof(struct rusage)) == -1) {
            perror("read_data");
        }
        else {
            printf("\nmax resident set size: %ld\npage reclaims: %ld\nsystem CPU time used: %ld\n",
                ru.ru_maxrss, ru.ru_minflt, (long int)ru.ru_stime.tv_usec);
        }

        sleep(1);
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

    if (pipe2(fd, O_NONBLOCK))
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