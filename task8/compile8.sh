#!/bin/bash

g++ -g -c ./task8-1.cpp
g++ -o task8-1 task8-1.o -lpthread

g++ -g -c ./task8-2.cpp
g++ -o task8-2 task8-2.o -lpthread

echo "Compilation complete!"
