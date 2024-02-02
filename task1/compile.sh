#!/bin/bash

g++ -c ./task1.cpp
g++ -o task1 task1.o -lpthread

echo "Compilation complete!"