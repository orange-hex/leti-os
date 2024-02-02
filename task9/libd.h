/*
Работа 9.4 -- Доступ к библиотекам ОС
Динамическая библиотека
Вариант с функцией getrusage
Митяков Д.А.
Группа 0324
*/

#include <sys/resource.h>

extern "C" int getgetrusage(__rusage_who_t, struct rusage*);