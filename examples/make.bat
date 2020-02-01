@echo off
SetLocal EnableDelayedExpansion

if "%1"=="clean" goto clean

set target=%1
set program=%2

if not "%target%"=="x86" if not "%target%"=="x64" if not "%mode%"=="clean" goto useage

if exist "vcvarsall.bat" (
    call "vcvarsall.bat" %target%
    goto begin
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %target%
    goto begin
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Preview\VC\Auxiliary\Build\vscarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Preview\VC\Auxiliary\Build\vscarsall.bat" %target%
    goto begin
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %target%
    goto begin
)
set /p VCVARSALLBAT=vcvarsall.bat directory:
call "%VCVARSALLBAT%" %target%

:begin
if not exist obj mkdir obj
set CXX=cl /nologo /D_CRT_SECURE_NO_WARNINGS /MT /EHsc /bigobj /O2 /Fo:obj\ /Fd:obj\
set LINK_ARGS=/link
call makelist.bat
goto :eof

:clean
if exist obj rmdir /S /Q obj
rm *.exe
goto :eof

:useage
echo compile: "make [x86/x64] <program>"
echo clean: "make clean"

EndLocal