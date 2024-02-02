#!/bin/bash

g++ -c ./task5.cpp
g++ -o task5-1 task5.o -lpthread
g++ -o task5-2 task5.o -lpthread

echo "Compilation complete!"
