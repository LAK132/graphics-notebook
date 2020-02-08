#include "X11/Xlib.h"
#include "X11/Xutil.h"

#ifndef STATIC_IMAGE_BLIT
#include "stdlib.h"
#endif
#include "stdio.h"
#include "string.h"

Status CreateImage(
  XImage *image,
  Display *display,
  Visual *visual,
  unsigned int depth,
  int format,
  int offset,
  char *data,
  unsigned int width,
  unsigned int height,
  int bitmap_pad,
  int bytes_per_line)
{
  if (image == NULL ||
      depth == 0 ||
      depth > 32 ||
      offset < 0 ||
      (format == XYBitmap && depth != 1) ||
      (format != XYBitmap &&
       format != XYPixmap &&
       format != ZPixmap) ||
      (bitmap_pad != 8 &&
       bitmap_pad != 16 &&
       bitmap_pad != 32))
  {
    /* Invalid arguments. */
    return 0;
  }

  int bits_per_pixel = 1;

  if (format == ZPixmap)
  {
    /*
     * User specified ZPixmap as the format, work out the bits
     * per pixel from the display's available formats.
     */
    int num_pixmap_formats = 0;
    XPixmapFormatValues *pixmap_formats =
      XListPixmapFormats(display, &num_pixmap_formats);

    while (num_pixmap_formats --> 0)
    {
      if (pixmap_formats[num_pixmap_formats].depth ==
          (int) depth)
      {
        bits_per_pixel =
          pixmap_formats[num_pixmap_formats].bits_per_pixel;
        break;
      }
    }

    if (num_pixmap_formats < 0)
    {
      if      (depth <= 4)  bits_per_pixel = 4;
      else if (depth <= 8)  bits_per_pixel = 8;
      else if (depth <= 32) bits_per_pixel = 32;
    }

    XFree(pixmap_formats);
  }

  {
    /*
     * Make sure the user has specified a valid number of bytes
     * per line, or automatically calculate it if the user
     * specified 0
     */
    int min_bytes_per_line;

    if (format == ZPixmap)
    {
      const int nbytes = bits_per_pixel * width;
      min_bytes_per_line =
        ((nbytes + (bitmap_pad - 1)) / bitmap_pad) *
        (bitmap_pad >> 3);
    }
    else
    {
      const int nbytes = width + offset;
      min_bytes_per_line =
        ((nbytes + (bitmap_pad - 1)) / bitmap_pad) *
        (bitmap_pad >> 3);
    }

    if (bytes_per_line == 0)
    {
      bytes_per_line = min_bytes_per_line;
    }
    else if (bytes_per_line < min_bytes_per_line)
    {
      /* User specified invalid bytes_per_line. */
      return 0;
    }
  }

  /* Size of the image. */
  image->width = width;
  image->height = height;

  /* Number of pixels offset in the X direction. */
  image->xoffset = offset;

  /* XYBitmap, XYPixmap or ZPixmap. */
  image->format = format;

  /* Pointer to image data. */
  image->data = data;

  /* Data byte order. LSBFirst or MSBFirst. */
  image->byte_order = XImageByteOrder(display);

  /* Quant. of scanline. 8, 16 or 32. */
  image->bitmap_unit = XBitmapUnit(display);

  /* Bitmap byte order. LSBFirst or MSBFirst. */
  image->bitmap_bit_order = XBitmapBitOrder(display);

  /* 8, 16 or 32. */
  image->bitmap_pad = bitmap_pad;

  /* Image depth. */
  image->depth = depth;

  /* Accelarator to next line. */
  image->bytes_per_line = bytes_per_line;

  /* Bits per pixel (ZPixmap). */
  image->bits_per_pixel = bits_per_pixel;

  if (visual != NULL)
  {
    /* Only used for ZPixmap format, derived from visual. */
    image->red_mask = visual->red_mask;
    image->blue_mask = visual->blue_mask;
    image->green_mask = visual->green_mask;
  }
  else
  {
    image->red_mask = image->blue_mask = image->green_mask = 0;
  }

  /* Unused. */
  image->obdata = NULL;

  return XInitImage(image);
}

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

  /* String to show in the title bar of the window. */
  const char *title = "X11/Xlib blit example";

  /* Set the window title. */
  XStoreName(display, window, title);

  /* Get the default visual for the screen. */
  /*
   * TODO: If the program is transfered between screens this
   * should be updated.
   */
  Visual *visual = DefaultVisual(display, screen);

  /* Only handle TrueColor Visuals. */
  if (visual->class != TrueColor)
  {
    fprintf(stderr, "Non TrueColor visuals not supported\n");
    return 1;
  }

  char pixels[200 * 200 * 4];

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

#ifdef STATIC_IMAGE_BLIT
  XImage image;
  Status create_image_status = CreateImage(
    &image,                         /* image */
    display,                        /* display */
    visual,                         /* visual */
    DefaultDepth(display, screen),  /* depth */
    ZPixmap,                        /* format */
    0,                              /* pixels start offset */
    pixels,                         /* data */
    200,                            /* width */
    200,                            /* height */
    32,                             /* bitmap pad */
    0                               /* bytes per line */
    /* 0 = contiguous, calculate automatically */
  );

  if (create_image_status == 0)
  {
    fprintf(stderr, "Failed to create image\n");
    return 1;
  }
#else
  /* Create an image to show in the window. */
  XImage *image = XCreateImage(
    display,                        /* display */
    visual,                         /* visual */
    DefaultDepth(display, screen),  /* depth */
    ZPixmap,                        /* format */
    0,                              /* pixels start offset */
    pixels,                         /* data */
    200,                            /* width */
    200,                            /* height */
    32,                             /* bitmap pad */
    0                               /* bytes per line */
    /* 0 = contiguous, calculate automatically */
  );

  if (image == NULL)
  {
    fprintf(stderr, "Failed to create image\n");
    return 1;
  }
#endif

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
        #ifdef STATIC_IMAGE_BLIT
        XImage *img = &image;
        #else
        XImage *img = image;
        #endif
        /* Blit image to the screen. */
        XPutImage(
          display,                    /* display */
          window,                     /* window */
          DefaultGC(display, screen), /* graphics controller */
          img,                        /* image */
          0,                          /* source x */
          0,                          /* source y */
          0,                          /* destination x */
          0,                          /* destination y */
          200,                        /* width */
          200                         /* height */
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

#ifndef STATIC_IMAGE_BLIT
  /*
   * XDestroyImage will attempt to free data, but we've
   * statically allocated it, so we must set data to NULL first.
   */
  image->data = NULL;
  XDestroyImage(image);
#endif

  /* Unmap (hide) the window. */
  XUnmapWindow(display, window);

  /* Destroy the window. */
  XDestroyWindow(display, window);

  /* Close the connection to the X server for display. */
  XCloseDisplay(display);
}
