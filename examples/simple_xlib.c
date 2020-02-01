#include "X11/Xlib.h"

#include "stdio.h"
#include "string.h"

int main()
{
  /*
   * Open a connection to the X server for a display.
   * On POSIX conforming system, NULL means use the DISPLAY
   * environment variable.
   * Otherwise the display name should take the form
   * "hostname:display_number.screen_number".
   */
  const char *display_name = NULL; /* ":0"; */
  Display *display = XOpenDisplay(display_name);

  /* XOpenDisplay returns NULL on failure. */
  if (display == NULL)
  {
    fprintf(stderr, "Failed to open Xlib display\n");
    return 1;
  }

  int screen = DefaultScreen(display);

  /* Create a window. */
  Window window = XCreateSimpleWindow(
    display,                      /* display */
    RootWindow(display, screen),  /* parent */
    10,                           /* x */
    10,                           /* y */
    200,                          /* width */
    200,                          /* height */
    1,                            /* border width */
    BlackPixel(display, screen),  /* border */
    WhitePixel(display, screen)   /* background */
  );

  /* Select the kind of events we are interestedn in. */
  long event_mask = ExposureMask | KeyPressMask;
  XSelectInput(display, window, event_mask);

  /* Map (show) the window. */
  XMapWindow(display, window);

  /*
   * When the user attempts to close the window via the (X)
   * button drawn by the system's window manager the program
   * will recieve a ClientMessage event with the WM_DELETE_WINDOW
   * atom.
   */
  Atom wm_delete_window = XInternAtom(
    display,            /* display */
    "WM_DELETE_WINDOW", /* atom name */
    0                   /* only if exists */
  );
  XSetWMProtocols(
    display,            /* display */
    window,             /* window */
    &wm_delete_window,  /* protocols */
    1                   /* count */
  );

  /* Message to draw in the window. */
  const char *title = "X11/Xlib window example";

  /* Set the window title. */
  XStoreName(display, window, title);

  /* Main event loop. */
  for (int running = 1; running;)
  {
    XEvent event;

    /* Get the next window event. */
    XNextEvent(display, &event);

    switch (event.type)
    {
      /* Draw/redraw the window. */
      case Expose: {
        /* Set the foreground colour to black. */
        XSetForeground(
          display,                    /* display */
          DefaultGC(display, screen), /* graphics controller */
          BlackPixel(display, screen) /* colour */
        );
        /* Draw a black rectangle. */
        XFillRectangle(
          display,                    /* display */
          window,                     /* drawable */
          DefaultGC(display, screen), /* graphics controller */
          10,                         /* x */
          10,                         /* y */
          180,                        /* width */
          180                         /* height */
        );

        /* Set the foreground colour to white. */
        XSetForeground(
          display,                    /* display */
          DefaultGC(display, screen), /* graphics controller */
          WhitePixel(display, screen) /* colour */
        );
        /* Draw a white rectangle. */
        XFillRectangle(
          display,                    /* display */
          window,                     /* drawable */
          DefaultGC(display, screen), /* graphics controller */
          20,                         /* x */
          20,                         /* y */
          160,                        /* width */
          160                         /* height */
        );

        /* Set the foreground colour to black. */
        XSetForeground(
          display,                    /* display */
          DefaultGC(display, screen), /* graphics controller */
          BlackPixel(display, screen) /* colour */
        );
        /* Draw the title string in the window. */
        XDrawString(
          display,                    /* display */
          window,                     /* drawable */
          DefaultGC(display, screen), /* graphics controller */
          30,                         /* x */
          100,                        /* y */
          title,                      /* string */
          strlen(title)               /* string length */
        );
      } break;

      /* Exit on key press */
      case KeyPress: {
        running = 0;
      } break;

      /*
       * Exit when the user presses the (X) button on the
       * window.
       */
      case ClientMessage: {
        if ((Atom)event.xclient.data.l[0] == wm_delete_window)
          running = 0;
      } break;

      default: break;
    }
  }

  /* Unmap (hide) the window. */
  XUnmapWindow(display, window);

  /* Destroy the window. */
  XDestroyWindow(display, window);

  /* Close the connection to the X server for display. */
  XCloseDisplay(display);
}
