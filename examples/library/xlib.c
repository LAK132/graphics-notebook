#include "../lak_window/lak_window.h"

#include "X11/Xlib.h"

#include "stdio.h"
#include "string.h"

Display *lak_get_display(lak_window_t);
int lak_get_screen(lak_window_t);
Window lak_get_window(lak_window_t);

/* Message to draw in the window. */
const char *title = "X11/Xlib library example";

int main()
{
  lak_window_t window = lak_create_window(0, 0, 200, 200);

  lak_show_window(window);

  lak_set_wtitle(window, L"X11/Xlib library example");

  printf("Number of screens %d\n",
         XScreenCount(lak_get_display(window)));

  lak_event_t event;
  int running = 1;
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

      case lak_event_type_motion:
        break;

      case lak_event_type_expose: {
        Display *display = lak_get_display(window);
        int screen = lak_get_screen(window);
        Window drawable = lak_get_window(window);
        /* Set the foreground colour to black. */
        XSetForeground(
          display,                    /* display */
          DefaultGC(display, screen), /* graphics context */
          BlackPixel(display, screen) /* colour */
        );
        /* Draw a black rectangle. */
        XFillRectangle(
          display,                    /* display */
          drawable,                   /* drawable */
          DefaultGC(display, screen), /* graphics context */
          10,                         /* x */
          10,                         /* y */
          180,                        /* width */
          180                         /* height */
        );

        /* Set the foreground colour to white. */
        XSetForeground(
          display,                    /* display */
          DefaultGC(display, screen), /* graphics context */
          WhitePixel(display, screen) /* colour */
        );
        /* Draw a white rectangle. */
        XFillRectangle(
          display,                    /* display */
          drawable,                   /* drawable */
          DefaultGC(display, screen), /* graphics context */
          20,                         /* x */
          20,                         /* y */
          160,                        /* width */
          160                         /* height */
        );

        /* Set the foreground colour to black. */
        XSetForeground(
          display,                    /* display */
          DefaultGC(display, screen), /* graphics context */
          BlackPixel(display, screen) /* colour */
        );
        /* Draw the title string in the window. */
        XDrawString(
          display,                    /* display */
          drawable,                   /* drawable */
          DefaultGC(display, screen), /* graphics context */
          30,                         /* x */
          100,                        /* y */
          title,                      /* string */
          strlen(title)               /* string length */
        );
      } break;

      default: break;
    }
  }

  lak_destroy_window(window);
}
