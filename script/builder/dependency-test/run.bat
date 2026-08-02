@echo off

set CXX=D:\dev\mingw32\bin\g++
set EXE=test.exe
set OPT=-g0 -O0
::set OPT=-O1 -O0
::set OPT=-g0 -s -Ofast
::-shared-libgcc
::-static-libgcc -static-libstdc++

cls
chcp 65001 1>NUL
del %EXE% 2>NUL

%CXX% -Wall -std=c++23 -pipe ^
-I. ^
%OPT% ^
-static-libgcc -static-libstdc++  ^
*.cpp funcs/*.cpp utils/*.cpp ^
-o %EXE% ^
-lstdc++exp

if %ERRORLEVEL% == 0 (
  %EXE%
)
