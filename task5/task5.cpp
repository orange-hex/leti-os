/*
Работа 5 -- Синхронизация процессов с помощью именованных семафоров
Вариант с циклом в main().
Возможно запустить несколько копий этой программы.
Митяков Д.А.
Группа 0324
*/

#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    printf(":: main start ::\n");

    bool flag = true;
    char symbol;  // Printed to a file
    char buf;
    ssize_t bytes;
    sem_t* semaphore;
    FILE* file;

    if (argc == 1) {
        // Get last char from filename
        char* arg = argv[0];
        symbol = arg[strlen(arg) - 1];

        printf("Using default symbol '%c'\n", symbol);
    } else {
        symbol = argv[1][0];
        printf("Symbol set to '%c'\n", symbol);
    }

    semaphore = sem_open("/task5sem", O_CREAT, 0644, 1);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    file = fopen("./task5file.txt", "a");
    if (file == NULL) {
        perror("fopen");
        sem_close(semaphore);
        sem_unlink("/task5sem");
        exit(1);
    }

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);  // Nonblock stdin

    while (flag) {
        sem_wait(semaphore);

        for (int i = 0; i < 10; i++) {
            fputc(symbol, file);
            fflush(file);
            putchar(symbol);
            fflush(stdout);
            usleep(500000);
        }

        printf("\n");

        sem_post(semaphore);

        bytes = read(STDIN_FILENO, &buf, 1);
        if (bytes > 0) {
            printf("Received keypress...\n");
            flag = false;
        }

        sleep(1);
    }

    fclose(file);
    sem_close(semaphore);
    sem_unlink("/task5sem");

    printf(":: main exit ::\n");
    return 0;
}