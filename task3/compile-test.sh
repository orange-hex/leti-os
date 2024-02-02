#!/bin/bash

g++ -c ./test.cpp
g++ -o test.out test.o -lpthread

echo "Compilation complete!"
