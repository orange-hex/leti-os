/*
Работа 9.4 -- Доступ к библиотекам ОС
Вариант с функцией getrusage
Митяков Д.А.
Группа 0324
*/

#include "libd.h"
#include <bits/types/struct_rusage.h>
#include <stdio.h>

int main() {
    struct rusage ru;

    if (getgetrusage(RUSAGE_SELF, &ru) == 0) {
        printf("\nmax resident set size: %ld\npage reclaims: %ld\n"
               "system CPU time used: %ld\n",
               ru.ru_maxrss, ru.ru_minflt, (long int)ru.ru_stime.tv_usec);
    } else
        perror("getrusage");

    return 0;
}