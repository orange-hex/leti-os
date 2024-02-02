#!/bin/bash

g++ -g -c ./task6-1.cpp
g++ -o task6-1 task6-1.o -lpthread

g++ -g -c ./task6-2.cpp
g++ -o task6-2 task6-2.o -lpthread

echo "Compilation complete!"
