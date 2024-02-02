/*
Работа 9.4 -- Доступ к библиотекам ОС
Реализация статической библиотеки
Вариант с функцией getrusage
Митяков Д.А.
Группа 0324
*/

#include "libs.h"

int getgetrusage(__rusage_who_t who, struct rusage* usage) {
    return getrusage(who, usage);
}