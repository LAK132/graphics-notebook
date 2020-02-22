#ifndef UNICODE
#define UNICODE
#endif

#include "win32_console.hpp"

#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <windowsx.h>
#include <strsafe.h>

#include <io.h>
#include <fcntl.h>

#include <stdio.h>

#include <iostream>
#include <string>

HBITMAP hBitmap = nullptr;
POINT cursor { 0, 0 };


// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
void ErrorExit(LPTSTR lpszFunction)
{
  // Retrieve the system error message for the last-error code

  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;
  DWORD dw = GetLastError();

  FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,             /* dwFlags */
    NULL,                                      /* lpSource */
    dw,                                        /* dwMessageId */
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* dwLanguageId */
    (LPTSTR) &lpMsgBuf,                        /* lpBuffer */
    0,                                         /* nSize */
    NULL                                       /* Arguments */
  );

  // Display the error message and exit the process

  lpDisplayBuf = (LPVOID)LocalAlloc(
    LMEM_ZEROINIT,
    (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) *
      sizeof(TCHAR)
  );

  StringCchPrintf(
    (LPTSTR)lpDisplayBuf,
    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
    L"%s failed with error %d: %s",
    lpszFunction,
    dw,
    lpMsgBuf
  );

  MessageBox(
    NULL,
    (LPCTSTR)lpDisplayBuf,
    L"Error",
    MB_OK
  );

  LocalFree(lpMsgBuf);
  LocalFree(lpDisplayBuf);
  ExitProcess(dw);
}

LRESULT CALLBACK WndProc(
  HWND hWnd,
  UINT msg,
  WPARAM wParam,
  LPARAM lParam)
{
  // HINSTANCE hInstance =
  //   (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

  switch (msg)
  {
    case WM_KEYDOWN:
    case WM_CLOSE: {
      DestroyWindow(hWnd);
    } return 0;

    case WM_CREATE: {
      // hBitmap = (HBITMAP)LoadImage(
      //   nullptr,        /* hInstance (null for LOADFROMFILE) */
      //   L"test.bmp",    /* name */
      //   IMAGE_BITMAP,   /* type */
      //   0,              /* width */
      //   0,              /* height */
      //   LR_LOADFROMFILE /* fuLoad */
      // );
      // if (!hBitmap) ErrorExit(L"LoadImage");

      HDC hDC = GetDC(hWnd);
      if (!hDC) ErrorExit(L"GetDC");

      HDC memDC = CreateCompatibleDC(hDC);
      if (!memDC) ErrorExit(L"CreateCompatibleDC");

      // NOTE: Not using memDC here, that would create a
      // monochrome bitmap.
      hBitmap = CreateCompatibleBitmap(
        hDC, /* device context handle */
        200, /* width */
        200  /* height */
      );
      if (!hBitmap) ErrorExit(L"CreateBitmap");

      HGDIOBJ oldBitmap = SelectObject(memDC, hBitmap);

      // BITMAP bitmap;
      // size_t bytes = GetObject(hBitmap, sizeof(bitmap), &bitmap);
      // if (bytes == 0) ErrorExit(L"GetObject");

      uint8_t pixels[200 * 200 * 4];

      for (int y = 0; y < 200; ++y)
      {
        for (int x = 0; x < 200; ++x)
        {
          pixels[(((y * 200) + x) * 4) + 0] = x;    /* blue */
          pixels[(((y * 200) + x) * 4) + 1] = y;    /* green */
          pixels[(((y * 200) + x) * 4) + 2] = 255;  /* red */
          pixels[(((y * 200) + x) * 4) + 3] = 0;    /* unused? */
        }
      }

      // bytes = GetBitmapBits(
      //   hBitmap,        /* bitmap handle */
      //   200 * 200 * 4,  /* no. bytes to copy */
      //   pixels          /* out buffer */
      // );
      // if (bytes == 0) ErrorExit(L"GetBitmapBits");

      size_t bytes = SetBitmapBits(
        hBitmap,        /* bitmap handle */
        200 * 200 * 4,  /* no. bytes to copy */
        pixels          /* out buffer */
      );
      if (bytes == 0) ErrorExit(L"SetBitmapBits");

      SelectObject(memDC, oldBitmap);

      DeleteObject(memDC);
      ReleaseDC(hWnd, hDC);
    } return 0;

    case WM_MOUSEMOVE: {
      BOOL bResult = RedrawWindow(
        hWnd,           /* hWnd */
        nullptr,        /* update region (rectangle) */
        nullptr,        /* update region handle */
        /* both nullptrs mean entire client area */
        RDW_INTERNALPAINT |
        RDW_INVALIDATE  /* flags */
      );
      if (!bResult) ErrorExit(L"RedrawWindow");
      cursor.x = GET_X_LPARAM(lParam);
      cursor.y = GET_Y_LPARAM(lParam);
    } return 0;

    case WM_DESTROY: {
      DeleteObject(hBitmap);
      PostQuitMessage(0 /* exit code */);
    } return 0;

/*
 * retrieves the bits of the specified compatible bitmap and
 * copies them into a buffer as a DIB using the specified
 * format.
 */
/*
int GetDIBits(
  HDC          hdc,
  HBITMAP      hbm,
  UINT         start,
  UINT         cLines,
  LPVOID       lpvBits,
  LPBITMAPINFO lpbmi,
  UINT         usage
);
*/

/*
 * sets the pixels in a compatible bitmap (DDB) using the color
 * data found in the specified DIB.
 */
/*
int SetDIBits(
  HDC              hdc,
  HBITMAP          hbm,
  UINT             start,
  UINT             cLines,
  const VOID       *lpBits,
  const BITMAPINFO *lpbmi,
  UINT             ColorUse
);
*/

    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hDC = BeginPaint(hWnd, &ps);
      if (hDC == nullptr) ErrorExit(L"BeginPaint");
      if (hBitmap == nullptr) return 1;

      HDC hDCBits = CreateCompatibleDC(hDC);
      if (hDCBits == nullptr) ErrorExit(L"CreateCompatibleDC");

      HGDIOBJ oldBitmap = SelectObject(hDCBits, hBitmap);

      BITMAP bitmap;
      auto bytes = GetObject(hBitmap, sizeof(bitmap), &bitmap);
      if (bytes == 0) ErrorExit(L"GetObject");

      // if ((GetDeviceCaps(hDCBits, RASTERCAPS) & RC_BITBLT) == 0)
      // {
      //   fprintf(conerr, "Cannot blit to device\n");
      //   MessageBox(
      //     hWnd,                       /* HWND */
      //     L"Cannot blit to device.",  /* message */
      //     L"Error",                   /* caption (title) */
      //     MB_OK                       /* type */
      //   );
      //   return 1;
      // }

      // if (int bpp = GetDeviceCaps(hDCBits, BITSPIXEL);
      //     bpp != (int)bitmap.bmBitsPixel)
      // {
      //   fprintf(conerr, "%dbpp != %dbpp\n", bpp, (int)bitmap.bmBitsPixel);
      //   MessageBox(
      //     hWnd,                         /* HWND */
      //     L"Differing bits per pixel.", /* message */
      //     L"Error",                     /* caption (title) */
      //     MB_OK                         /* type */
      //   );
      //   return 1;
      // }

      auto blitSuccess = BitBlt(
        hDC,              /* destination device context handle */
        cursor.x,         /* destination x */
        cursor.y,         /* destination y */
        bitmap.bmWidth,   /* destination width */
        bitmap.bmHeight,  /* destination height */
        hDCBits,          /* source device context handle */
        0,                /* source x */
        0,                /* source y */
        SRCCOPY           /* raster-operation */
      );
      if (blitSuccess == 0) ErrorExit(L"BitBlt");

      SelectObject(hDCBits, oldBitmap);

      DeleteDC(hDCBits);

      EndPaint(hWnd, &ps);
    } return 0;

    default:
      /* fall back to the default window proc */
      return DefWindowProc(hWnd, msg, wParam, lParam);
  }
}

int WINAPI wWinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  PWSTR lpCmdLine,
  int nCmdShow)
{
  RedirectStdIOToConsole();

  WNDCLASSW window_class = {}; /* zero initialise */

  window_class.lpfnWndProc = &WndProc;
  window_class.hInstance = hInstance;
  window_class.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
  window_class.hCursor = LoadCursor(hInstance, IDC_ARROW);
  window_class.lpszClassName  = L"Win32 example window class";

  ATOM wClass = RegisterClass(&window_class);
  if (!wClass) ErrorExit(L"RegisterClass.");

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

  if (!hwnd) ErrorExit(L"CreateWindowEx.");

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

    /*
     * Tell Windows to call the window procedure of the window
     * that is the target of this message.
     */
    DispatchMessage(&msg);
  }

  DestroyWindow(hwnd);

  UnregisterClass(window_class.lpszClassName, hInstance);

  return msg.wParam;
}