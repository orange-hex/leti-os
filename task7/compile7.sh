#!/bin/bash

g++ -g -c ./task7-1.cpp
g++ -o task7-1 task7-1.o -lpthread

g++ -g -c ./task7-2.cpp
g++ -o task7-2 task7-2.o -lpthread

echo "Compilation complete!"
