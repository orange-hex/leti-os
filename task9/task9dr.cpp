/*
Работа 9.4 -- Доступ к библиотекам ОС
Вариант с функцией getrusage
Митяков Д.А.
Группа 0324
*/

#include "libd.h"
#include <bits/types/struct_rusage.h>
#include <dlfcn.h>
#include <stdio.h>

int main() {
    struct rusage ru;
    void* handle;
    char* error;
    int (*dgetrusage)(__rusage_who_t, struct rusage*);

    handle = dlopen("libd.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\nMake sure that LD_LIBRARY_PATH is set correctly",
                dlerror());
        return 1;
    }

    dlerror(); // Clear existing errors

    dgetrusage =
        (int (*)(__rusage_who_t, struct rusage*)) dlsym(handle, "getgetrusage");

    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }

    dgetrusage(RUSAGE_SELF, &ru);

    printf("\nmax resident set size: %ld\n"
           "page reclaims: %ld\n"
           "system CPU time used: %ld\n",
           ru.ru_maxrss, ru.ru_minflt, (long int)ru.ru_stime.tv_usec);

    dlclose(handle);

    return 0;
}