#!/bin/bash

g++ -g -c -o task10.o task10.cpp
g++ -o task10.out task10.o -lev

rm task10.o

echo "Done!"
