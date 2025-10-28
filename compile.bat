@echo off
windres --input application/windows/manifest.rc --output application/windows/manifest.res --output-format=coff
g++ main.cpp -o p.exe application/windows/manifest.res -D WINDOWS -O3 -lSDL2 -std=c++20
