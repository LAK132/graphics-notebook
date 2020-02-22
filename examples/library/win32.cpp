#ifndef UNICODE
#define UNICODE
#endif

#include "../lak_window/lak_window.h"

#include <windows.h>
#include <wingdi.h>

#include <stdio.h>

HWND lak_get_hwnd(lak_window_t window);

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    PWSTR lpCmdLine,
                    int nCmdShow)
{
  lak_window_t window = lak_create_window(0, 0, 200, 200);

  lak_show_window(window);

  lak_set_wtitle(window, L"Win32 library example");

  bool running = true;
  lak_event_t event;
  while (running && lak_get_event(window, &event))
  {
    switch (event.type)
    {
      case lak_event_type_keydown:
        running = 0;
        break;

      case lak_event_type_close:
        // User has requested that the program quits.
        running = 0;
        break;

      // case lak_event_type_quit:
      //   // End of messages.
      //   running = 0;
      //   break;

      case lak_event_type_motion:
        break;

      case lak_event_type_expose: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(lak_get_hwnd(window), &ps);

        HBRUSH black_brush = CreateSolidBrush(RGB(0x00, 0x00, 0x00));
        HBRUSH white_brush = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));

        RECT rect { 0, 0, 200, 200};
        FillRect(hdc, &rect, white_brush);

        rect = RECT { 10, 10, 190, 190 };
        FillRect(hdc, &rect, black_brush);

        rect = RECT { 20, 20, 180, 180 };
        FillRect(hdc, &rect, white_brush);

        rect = RECT { 20, 90, 180, 110 };
        DrawText(
          hdc,                      /* hdc */
          L"Win32 library example", /* lpchText */
          -1, /* cchText (text length,-1 = null terminated) */
          &rect,                    /* lprc */
          DT_CENTER                 /* format */
        );

        DeleteObject(black_brush);
        DeleteObject(white_brush);

        EndPaint(lak_get_hwnd(window), &ps);
      } break;

      default: break;
    }
  }

  lak_destroy_window(window);

  return 0;
}