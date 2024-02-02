/*
Работа 4 -- Создание и уничтожение процессов
Вариант 5 -- Функция execv
Программа 2 -- Родительская программа, вызываюющая программу 1 с параметрами
Митяков Д.А.
Группа 0324
*/

#include <sched.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    printf(":: main start ::\n");

    if (argc == 1)
        printf("NOTE: No arguments passed.\n");

    pid_t child = fork();

    if (child == 0) {
        // Child process
        if (execv("./task4-child", argv) == -1)
            perror("execv");
    } else if (child > 0) {
        // Parent process
        printf(":: parent process start ::\n");
        printf("self pid: %d\nparent pid: %d\nchild pid: %d\n",
            getpid(), getppid(), child);

        int status;

        while (waitpid(child, &status, WNOHANG) == 0) {
            printf("parent waiting for child (%d)...\n", child);
            usleep(500000);  // 500 ms
        }

        printf("child exit code: %d\n", WEXITSTATUS(status));

        printf(":: parent process exit ::\n");
    } else {
        perror("fork");
        return 1;
    }

    printf(":: main exit ::\n");
    return 0;
}