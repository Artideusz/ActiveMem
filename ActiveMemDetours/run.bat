@echo off
rmdir build /q /s
cmake -S . -B build
cmake --build build
@REM python .\tools\convert_winapi_defs\main.py kernel32.dll VirtualAlloc VirtualFree > src/true_funcs.h
pause
.\build\Debug\withdll.exe /d:build/Debug/main.dll %*