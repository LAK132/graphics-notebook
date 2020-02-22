#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wingdi.h>

#include <stdio.h>

void paint_window(HWND hwnd)
{
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);

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
    L"Win32 window example",  /* lpchText */
    -1, /* cchText (text length,-1 = null terminated) */
    &rect,                    /* lprc */
    DT_CENTER                 /* format */
  );

  DeleteObject(black_brush);
  DeleteObject(white_brush);

  EndPaint(hwnd, &ps);
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

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    PWSTR lpCmdLine,
                    int nCmdShow)
{
  WNDCLASSW window_class = {}; /* zero initialise */

  window_class.lpfnWndProc = &WindowProc;
  window_class.hInstance = hInstance;
  window_class.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
  window_class.hCursor = LoadCursor(hInstance, IDC_ARROW);
  window_class.lpszClassName  = L"Win32 example window class";

  ATOM wClass = RegisterClass(&window_class);
  if (!wClass)
  {
    MessageBox(
      NULL,                                /* HWND */
      L"Failed to create a window class.", /* message */
      L"Error",                            /* caption (title) */
      MB_OK                                /* type */
    );
    return 1;
  }

  HWND hwnd = CreateWindowEx(
    0,                          /* styles */
    window_class.lpszClassName, /* class name */
    L"Win32 window example",    /* window name */
    WS_OVERLAPPEDWINDOW,        /* style */
    CW_USEDEFAULT,              /* x */
    CW_USEDEFAULT,              /* y */
    215,                        /* width */
    240,                        /* height */
    nullptr,                    /* parent */
    nullptr,                    /* menu */
    hInstance,                  /* hInstance */
    nullptr                     /* user data */
  );

  if (!hwnd)
  {
    MessageBox(
      NULL,                           /* HWND */
      L"Failed to create a window.",  /* message */
      L"Error",                       /* caption (title) */
      MB_OK                           /* type */
    );
    return 1;
  }

  ShowWindow(hwnd, SW_SHOWNORMAL);

  MSG msg = {};
  while (GetMessage(
           &msg,  /* message */
           hwnd,  /* HWND */
           0,     /* filter min */
           0      /* filter max */
         ) > 0)
  {
    /*
     * Translate keystrokes into characters.
     * Must be called before DispatchMessage.
     */
    TranslateMessage(&msg);

    switch (msg.message)
    {
      case WM_KEYDOWN:
      case WM_CLOSE:
        /* Triggers the WM_QUIT message which stops GetMessage */
        PostQuitMessage(0 /* exit code */);
        break;

      case WM_DESTROY:
        PostQuitMessage(0 /* exit code */);
        break;

      case WM_PAINT:
        paint_window(msg.hwnd);
        break;

      default:
        /* fall back to the default window proc */
        DefWindowProc(
          msg.hwnd,
          msg.message,
          msg.wParam,
          msg.lParam);
        break;
    }
  }

  DestroyWindow(hwnd);

  UnregisterClass(window_class.lpszClassName, hInstance);

  return msg.wParam;
}