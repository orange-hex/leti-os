#!/bin/bash

g++ -c ./task4-child.cpp
g++ -o task4-child.out task4-child.o -lpthread

g++ -c ./task4-parent.cpp
g++ -o task4-parent.out task4-parent.o -lpthread

echo "Compilation complete!"
