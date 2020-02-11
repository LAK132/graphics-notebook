// https://stackoverflow.com/a/3009145
// http://www.halcyon.com/~ast/dload/guicon.htm

#ifndef UNICODE
#define UNICODE
#endif

#include "win32_console.hpp"

#include <strsafe.h>
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

FILE *conout  = nullptr;
FILE *conin   = nullptr;
FILE *conerr  = nullptr;

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

// This function SHOULD rebind stdout, stdin, stderr (Stream) to
// the console stream for StdHandle, but for whatever fucking
// reason does NOT do that.
/*
void RedirectStreamToConsole(
  DWORD StdHandle,
  FILE *Stream,
  const char *Mode)
{
  intptr_t stdHandle = (intptr_t)GetStdHandle(StdHandle);

  int consoleHandle = _open_osfhandle(stdHandle, _O_TEXT);

  FILE *fp = _fdopen(consoleHandle, Mode);
  if (fp == nullptr) abort();

  *Stream = *fp;

  if (setvbuf(Stream, NULL, _IONBF, 0) != 0) abort();
}
*/

// This version is used to bind to conout, conin, conerr instead.
void RedirectStreamToConsole(
  DWORD StdHandle,
  FILE **Stream,
  const char *Mode)
{
  intptr_t stdHandle = (intptr_t)GetStdHandle(StdHandle);

  int consoleHandle = _open_osfhandle(stdHandle, _O_TEXT);

  *Stream = _fdopen(consoleHandle, Mode);
  if (Stream == nullptr) abort();

  if (setvbuf(*Stream, NULL, _IONBF, 0) != 0) abort();
}

void RedirectStdIOToConsole()
{
  // allocate a console for this app
  AllocConsole();
  // Don't check if this fails, because it always fails when
  // cross compiling on Linux despite the console still working
  // fine.

  // set the screen buffer to be big enough to let us scroll text
  CONSOLE_SCREEN_BUFFER_INFO coninfo;
  GetConsoleScreenBufferInfo(
    GetStdHandle(STD_OUTPUT_HANDLE),
    &coninfo
  );

  coninfo.dwSize.Y = MAX_CONSOLE_LINES;

  if (SetConsoleScreenBufferSize(
        GetStdHandle(STD_OUTPUT_HANDLE),
        coninfo.dwSize
      ) == 0) abort();

  if (SetConsoleTitle(L"Win32 Console Title") == 0) abort();

  // redirect unbuffered STDOUT to the console
  // RedirectStreamToConsole(
  //   STD_OUTPUT_HANDLE,
  //   stdout,
  //   "w");
  RedirectStreamToConsole(
    STD_OUTPUT_HANDLE,
    &conout,
    "w");

  // redirect unbuffered STDIN to the console
  // RedirectStreamToConsole(
  //   STD_INPUT_HANDLE,
  //   stdin,
  //   "r");
  RedirectStreamToConsole(
    STD_INPUT_HANDLE,
    &conin,
    "r");

  // redirect unbuffered STDERR to the console
  // RedirectStreamToConsole(
  //   STD_ERROR_HANDLE,
  //   stderr,
  //   "w");
  RedirectStreamToConsole(
    STD_ERROR_HANDLE,
    &conerr,
    "w");

  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
  // point to console as well
  // std::ios::sync_with_stdio();
}
