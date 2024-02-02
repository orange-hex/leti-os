#!/bin/bash

g++ -c ./task5.cpp
g++ -o task5-1.out task5.o -lpthread
g++ -o task5-2.out task5.o -lpthread

echo "Compilation complete!"
