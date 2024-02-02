/*
Работа 2 -- Синхронизация потоков с помощью мьютексов и неименованных семафоров
Задание 3 -- С использованием блокировки на время операции захвата мьютекса
Митяков Д.А.
Группа 0324
*/

#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

typedef struct {
    int flag;
    char symbol;
} targs;

pthread_mutex_t mutex;

void* procedure(void* args) {
    targs *arg = (targs*)args;
    struct timespec tp;  // Each thread gets its own timer

    printf("procedure%c() start\n", arg->symbol);

    while (arg->flag != 1) {
        clock_gettime(CLOCK_REALTIME, &tp);
        tp.tv_sec += 1;

        int errcode = pthread_mutex_timedlock(&mutex, &tp);

        if (!errcode) {

            for (int i = 0; i < 4; i++) {
                putchar(arg->symbol);
                fflush(stdout);
                sleep(1);
            }

            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
        else {
            printf("\nprocedure%c() timedlock error: %s\n", arg->symbol, strerror(errcode));
        }
    }

    printf("procedure%c() exit\n", arg->symbol);
    pthread_exit(NULL);
}

int main() {
    printf("main() start\n");

    targs args1 = { 0, 'A' };
    targs args2 = { 0, 'B' };

    pthread_t proc1;
    pthread_t proc2;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&proc1, NULL, &procedure, &args1);
    pthread_create(&proc2, NULL, &procedure, &args2);

    printf("main() awaits keypress...\n");
    getchar();
    printf("main() received keypress\n");

    args1.flag = 1;
    args2.flag = 1;

    pthread_join(proc1, NULL);
    pthread_join(proc2, NULL);

    pthread_mutex_destroy(&mutex);

    printf("main() exit\n");

    return 0;
}