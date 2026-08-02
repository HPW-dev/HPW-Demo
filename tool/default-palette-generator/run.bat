@echo off

set CXX=D:\dev\mingw32\bin\g++
set EXE=test.exe
set OPT=-g0 -O0
::set OPT=-g0 -s -Ofast

cls
chcp 65001 1>NUL
del %EXE% 2>NUL
del result.ppm 2>NUL

%CXX% -Wall -std=c++23 -pipe ^
-I. ^
%OPT% ^
-static-libgcc -static-libstdc++ ^
*.cpp ^
-o %EXE%

%EXE%
result.ppm
