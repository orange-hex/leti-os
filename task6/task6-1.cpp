/*
Работа 6 -- Взаимодействие процессов через разделяемую память
Вариант с памятью стандарта POSIX и функцией getrusage
Программа 1 -- Читает данные из разделяемой памяти
Митяков Д.А.
Группа 0324
*/

#include <bits/types/struct_rusage.h>
#include <cstddef>
#include <fcntl.h>
#include <sched.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

sem_t* sem_read;
sem_t* sem_write;
struct rusage* mm; // Mapped memory
void* ma = NULL;   // Address of mapping
int fd;            // Shared memory object descriptor
const size_t SIZE = sizeof(struct rusage);

void free_res() {
    sem_close(sem_read);
    sem_unlink("/semread6");
    sem_close(sem_write);
    sem_unlink("/semwrite6");
    shm_unlink("/shm6");
    munmap(NULL, SIZE);
    printf("Resources freed\n");
}

void interrupt_handler(int signo) {
    printf("\nInterrupt signal recieved\n");
    free_res();
    exit(0);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, interrupt_handler);
    printf(":: main start ::\n");

    bool flag = true;
    char buf;
    ssize_t bytes;
    struct rusage ru;

    sem_read = sem_open("/semread6", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    sem_write = sem_open("/semwrite6", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    if (sem_read == SEM_FAILED || sem_write == SEM_FAILED) {
        perror("sem_open");
        free_res();
        exit(1);
    }

    fd = shm_open("/shm6", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        free_res();
        exit(1);
    }

    if (ftruncate(fd, SIZE) == -1) {
        perror("ftruncate");
        free_res();
        exit(1);
    }

    mm = (struct rusage*)mmap(ma, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                              0);
    if (*(int*)mm == -1) {
        perror("mmap");
        free_res();
        exit(1);
    }

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // Nonblock stdin

    printf("Awaiting write...\n");
    while (flag) {
        sem_wait(sem_write);

        memcpy(&ru, mm, SIZE);

        printf("\nmax resident set size: %ld\npage reclaims: %ld\nsystem CPU "
               "time used: %ld\n",
               ru.ru_maxrss, ru.ru_minflt, (long int)ru.ru_stime.tv_usec);

        sem_post(sem_read);

        bytes = read(STDIN_FILENO, &buf, 1);
        if (bytes > 0) {
            printf("Received keypress...\n");
            flag = false;
        }

        sleep(1);
    }

    free_res();

    printf(":: main exit ::\n");
    return 0;
}
