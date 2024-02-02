/*
Работа 1 -- Создание и Уничтожение Потоков
Митяков Д.А.
Группа 0324
*/

#include <cstddef>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

typedef struct {
    int flag;
    char symbol;
} targs;

void* procedure(void* args) {
    targs *arg = (targs*)args;
    
    printf("procedure%c() start\n", arg->symbol);

    while (arg->flag != 1) {
        putchar(arg->symbol);
        fflush(stdout);
        sleep(1);
    }

    printf("procedure%c() exit\n", arg->symbol);
    pthread_exit((void*)0);
}

int main() {
    printf("main() start\n");
    
    targs args1 = { 0, 'A' };
    targs args2 = { 0, 'B' };

    int* exit1;
    int* exit2;

    pthread_t proc1;
    pthread_t proc2;

    pthread_create(&proc1, NULL, &procedure, &args1);
    pthread_create(&proc2, NULL, &procedure, &args2);

    printf("main() awaits keypress...\n");
    getchar();
    printf("main() received keypress\n");

    args1.flag = 1;
    args2.flag = 1;

    pthread_join(proc1, (void**)&exit1);
    pthread_join(proc2, (void**)&exit2);

    printf("procedure%c() exit code: %p\n", args1.symbol, exit1);
    printf("procedure%c() exit code: %p\n", args2.symbol, exit2);

    printf("main() exit\n");

    return 0;
}
