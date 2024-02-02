/*
Работа 10.2 -- Событийное программирование на уровне ос
Задание 1 -- В работе с неименованными каналами заменить потоки записи и
             чтения на обработчики событий. Запись выполнять по событиям
             от таймера, а чтение по событиям ввода/вывода.
Митяков Д.А.
Группа 0324
*/

#include <ev.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

ev_io stdin_watcher;    // User input watcher
ev_io pipe_watcher;     // Read
ev_timer write_watcher; // Write

int fd[2];

static void stdin_cb(EV_P_ ev_io* w, int revents) {
    puts("User input received");
    ev_io_stop(EV_A_ w);
    ev_break(EV_A_ EVBREAK_ALL);
}

static void pipe_cb(EV_P_ ev_io* w, int revents) {
    struct rusage ru;
    if (read(fd[0], &ru, sizeof(struct rusage)) > 0)
        printf("max resident set size: %ld\n"
               "page reclaims: %ld\n"
               "system CPU time used: %ld\n",
               ru.ru_maxrss, ru.ru_minflt, (long int)ru.ru_stime.tv_usec);
}

static void write_cb(EV_P_ ev_timer* w, int revents) {
    puts(":: write ::");

    struct rusage ru;
    if (getrusage(RUSAGE_SELF, &ru) == 0)
        write(fd[1], &ru, sizeof(struct rusage));

    ev_timer_again(loop, &write_watcher);
}

int main() {
    if (pipe(fd))
        return 1;

    struct ev_loop* loop = EV_DEFAULT;

    // Watch stdin to become readable
    ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    // Begin watching for fd
    ev_io_init(&pipe_watcher, pipe_cb, fd[0], EV_READ);
    ev_io_start(loop, &pipe_watcher);

    // Begin the write interval timer
    ev_init(&write_watcher, write_cb);
    write_watcher.repeat = 1.;
    ev_timer_again(loop, &write_watcher);

    ev_run(loop, 0);

    close(fd[0]);
    close(fd[1]);

    puts(":: main exit ::");
    return 0;
}