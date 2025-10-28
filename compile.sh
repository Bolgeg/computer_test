#!/bin/bash
g++ main.cpp -o p -O3 -lSDL2 -pthread -Wl,-rpath . -std=c++20
