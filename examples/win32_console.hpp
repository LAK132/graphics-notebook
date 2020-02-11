#include <windows.h>
#include <stdio.h>

#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#ifndef WIN32_CONSOLE_HPP
#define WIN32_CONSOLE_HPP

extern FILE *conout;
extern FILE *conin;
extern FILE *conerr;

void RedirectStreamToConsole(
  DWORD StdHandle,
  FILE *Stream,
  const char *Mode);

void RedirectStdIOToConsole();

#endif // WIN32_CONSOLE_HPP