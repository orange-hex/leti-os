#!/bin/bash

g++ -c ./test.cpp
g++ -o test test.o -lpthread

echo "Compilation complete!"
