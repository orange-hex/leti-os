/*
Работа 4 -- Создание и уничтожение процессов
Программа 1 -- Дочерняя программа, выводящая на экран принятые аргументы
Митяков Д.А.
Группа 0324
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    printf(":: child process start ::\n");
    printf("self pid: %d\nparent pid: %d\nargs: ", getpid(), getppid());

    sleep(1);
    
    for (int i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }

    printf("\n:: child process exit ::\n");
    exit(69);  // More noticeable value
}