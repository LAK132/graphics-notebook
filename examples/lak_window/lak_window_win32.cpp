#include "lak_window.h"
#include "lak_memory.h"

#include <windows.h>
#include <wingdi.h>
#include <windowsx.h>

#include <stdio.h>

struct _lak_window
{
  HINSTANCE hInstance;
  WNDCLASS wndClass;
  HWND hWnd;
};

HINSTANCE lak_get_hinstance(lak_window_t window)
{
  return window->hInstance;
}

WNDCLASS lak_get_wndclass(lak_window_t window)
{
  return window->wndClass;
}

HWND lak_get_hwnd(lak_window_t window)
{
  return window->hWnd;
}

LRESULT CALLBACK WindowProc(
  HWND hWnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam)
{
  switch (Msg)
  {
    case WM_CREATE:
    case WM_CLOSE:
    case WM_DESTROY:
      PostMessage(hWnd, Msg, wParam, lParam);
      return 0;

    default:
      /* fall back to the default window proc */
      return DefWindowProc(hWnd, Msg, wParam, lParam);
  }
}

lak_window_t lak_create_window(
  int pos_x,
  int pos_y,
  unsigned int width,
  unsigned int height)
{
  lak_window_t window =
    (lak_window_t)lak_malloc(sizeof(_lak_window));
  if (!window) return nullptr;

  window->hInstance = GetModuleHandle(nullptr);

  window->wndClass = {}; /* zero initialise */

  // window->wndClass.lpfnWndProc = &DefWindowProc;
  window->wndClass.lpfnWndProc = &WindowProc;
  window->wndClass.hInstance = window->hInstance;
  window->wndClass.hIcon =
    LoadIcon(window->hInstance, IDI_APPLICATION);
  window->wndClass.hCursor =
    LoadCursor(window->hInstance, IDC_ARROW);
  window->wndClass.lpszClassName = TEXT("Win32 Window Class");

  ATOM wClass = RegisterClass(&window->wndClass);
  if (!wClass)
  {
    lak_free(window);
    return nullptr;
  }

  window->hWnd = CreateWindowEx(
    0,                              /* styles */
    window->wndClass.lpszClassName, /* class name */
    nullptr,                        /* window name */
    WS_OVERLAPPEDWINDOW,            /* style */
    pos_x,                          /* x */
    pos_y,                          /* y */
    width,                          /* width */
    height,                         /* height */
    nullptr,                        /* parent */
    nullptr,                        /* menu */
    window->hInstance,              /* hInstance */
    nullptr                         /* user data */
  );

  if (!window->hWnd)
  {
    UnregisterClass(
      window->wndClass.lpszClassName,
      window->hInstance);
    lak_free(window);
    return nullptr;
  }

  return window;
}

void lak_show_window(lak_window_t window)
{
  ShowWindow(window->hWnd, SW_SHOWNORMAL);
}

void lak_set_title(lak_window_t window, const wchar_t *title)
{
  SetWindowTextW(window->hWnd, title);
}

void lak_set_title(lak_window_t window, const char *title)
{
  SetWindowTextA(window->hWnd, title);
}

int lak_get_event(lak_window_t window, lak_event_t *event)
{
  MSG msg;
  bool result = GetMessage(&msg, window->hWnd, 0, 0) >= 0;
  /*
   * Translate keystrokes into characters.
   * Must be called before DispatchMessage.
   */
  TranslateMessage(&msg);

  switch(msg.message)
  {
    case WM_KEYDOWN:
      event->type = lak_event_type_keydown;
      break;

    case WM_KEYUP:
      event->type = lak_event_type_keyup;
      break;

    case WM_MOUSEMOVE:
      event->type = lak_event_type_motion;
      event->motion.x = GET_X_LPARAM(msg.lParam);
      event->motion.y = GET_Y_LPARAM(msg.lParam);
      break;

    // Interceptable close event.
    case WM_CLOSE:
      event->type = lak_event_type_close;
      break;

    // // No more messages will follow this.
    // case WM_QUIT:
    //   event->type = lak_event_type_quit;
    //   break;

    case WM_CREATE:
    case WM_DESTROY:
      event->type = lak_event_type_nil;
      break;

    case WM_PAINT:
      event->type = lak_event_type_expose;
      break;

    default:
      event->type = lak_event_type_nil;
      /* fall back to the default window proc */
      DefWindowProc(
        msg.hwnd,
        msg.message,
        msg.wParam,
        msg.lParam);
      break;
  }
  return result;
}

void lak_destroy_window(lak_window_t window)
{
  DestroyWindow(window->hWnd);

  UnregisterClass(
    window->wndClass.lpszClassName,
    window->hInstance);

  lak_free(window);
}