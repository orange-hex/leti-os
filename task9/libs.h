/*
Работа 9.4 -- Доступ к библиотекам ОС
Статическая библиотека
Вариант с функцией getrusage
Митяков Д.А.
Группа 0324
*/

#include <sys/resource.h>

int getgetrusage(__rusage_who_t, struct rusage*);