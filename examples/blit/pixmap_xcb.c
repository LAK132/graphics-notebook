#include "xcb/xcb.h"

#ifdef USE_XCB_IMAGE
/* requires libxcb-image0-dev */
#include "xcb/xcb_image.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef USE_XCB_IMAGE
int create_image(
  xcb_image_t *image,
  xcb_connection_t *connection,
  uint8_t depth,
  xcb_image_format_t format,
  uint8_t *data,
  uint16_t width,
  uint16_t height,
  uint8_t bitmap_pad)
{
  if (image == NULL ||
      connection == NULL ||
      depth == 0 ||
      depth > 32 ||
      (format == XCB_IMAGE_FORMAT_XY_BITMAP && depth != 1) ||
      (format != XCB_IMAGE_FORMAT_XY_BITMAP &&
       format != XCB_IMAGE_FORMAT_XY_PIXMAP &&
       format != XCB_IMAGE_FORMAT_Z_PIXMAP) ||
      (bitmap_pad != 8 &&
       bitmap_pad != 16 &&
       bitmap_pad != 32))
  {
    /* Invalid arguments. */
    return 0;
  }

  const xcb_setup_t *setup = xcb_get_setup(connection);

  uint8_t bits_per_pixel = 1;

  if (format == XCB_IMAGE_FORMAT_Z_PIXMAP)
  {
    /*
     * User specified Z-pixmap as the format, work out the bits
     * per pixel from the display's available formats.
     */
    xcb_format_t *format = xcb_setup_pixmap_formats(setup);
    int num_pixmap_formats =
      xcb_setup_pixmap_formats_length(setup);

    while (num_pixmap_formats --> 0)
    {
      if (format[num_pixmap_formats].depth == depth)
      {
        bits_per_pixel =
          format[num_pixmap_formats].bits_per_pixel;
        break;
      }
    }

    if (num_pixmap_formats < 0)
    {
      if      (depth <= 4)  bits_per_pixel = 4;
      else if (depth <= 8)  bits_per_pixel = 8;
      else if (depth <= 32) bits_per_pixel = 32;
    }
  }

  /* Size of the image. */
  image->width = width;
  image->height = height;

  /*
   * XCB_IMAGE_FORMAT_XY_BITMAP, XCB_IMAGE_FORMAT_XY_PIXMAP or
   * XCB_IMAGE_FORMAT_Z_PIXMAP.
   */
  image->format = format;

  /* Right pad in bits. 8, 16 or 32. */
  image->scanline_pad = bitmap_pad;
  /* should this be format->scanline_pad ? */

  /*
   * Depth in bits.
   * Valid depths are 1, 4, 8, 16 and 24 for Z-pixmap,
   * 1 for XY-bitmap and anything for XY-pixmap.
   */
  image->depth = depth;

  /*
   * Storage per pixel in bits.
   * Must be >= depth.
   * 1, 4, 8, 16, 24 or 32 for Z-pixmap.
   * 1 for XY-bitmap.
   * Anything for XY-pixmap.
   */
  image->bpp = bits_per_pixel;

  /*
   * Scanline unit in bits for XY-bitmapd and XY-pixmap and for
   * bpp = 1, for which valid values are 8, 16, 32.
   * Otherwise max(8, bpp).
   * Must be >= bpp.
   */
  image->unit = setup->bitmap_format_scanline_unit;

  /* Rarely used. "Currently only set by xcb_image_get()". */
  image->plane_mask = 0;

  /*
   * Component byte order for Z-pixmap.
   * Nybble order for Z-pixmap when bpp = 4.
   * Order of scanline unit for XY-bitmap and XY-pixmap.
   */
  image->byte_order =
    (xcb_image_order_t)setup->image_byte_order;

  /* Bit ordering for XY-bitmap and XY-pixmap. */
  image->bit_order =
    (xcb_image_order_t)setup->bitmap_format_bit_order;

  /* Unused. */
  image->base = NULL;

  /* Pointer to image data. */
  image->data = data;

  /* Update size and stride. */
  xcb_image_annotate(image);

  return 1;
}
#endif

/*
 * Helper function for getting atoms.
 * Getting atoms requires memory managment, so by containing
 * it in this function we reduce the chance of leaking.
 */
xcb_atom_t get_atom(
  xcb_connection_t *connection,
  const char *name)
{
  xcb_intern_atom_cookie_t cookie =
    xcb_intern_atom_unchecked(
      connection,   /* connection */
      0,            /* only if exists */
      strlen(name), /* length */
      name          /* name */
    );

  xcb_intern_atom_reply_t *cookie_reply =
    xcb_intern_atom_reply(
      connection, /* connection */
      cookie,     /* cookie */
      NULL        /* error out (?) */
    );

  xcb_atom_t result = cookie_reply->atom;

  free(cookie_reply);

  return result;
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
  xcb_connection_t *connection = xcb_connect(
    display_name, /* display name */
    NULL          /* preferred screen number (optional) */
  );

  if (xcb_connection_has_error(connection))
  {
    fprintf(stderr, "Failed to open XCB display\n");
    return 1;
  }

  /* Get the first screen. */
  xcb_screen_t *screen = xcb_setup_roots_iterator(
    xcb_get_setup(connection)).data;

  /* Create black graphics context. */
  xcb_gcontext_t black_gcontext = xcb_generate_id(connection);
  {
    uint32_t values[] = {
      screen->black_pixel,
      screen->white_pixel,
      0
    };
    xcb_create_gc(
      connection,                 /* connection */
      black_gcontext,             /* context id */
      screen->root,               /* drawable */
      XCB_GC_FOREGROUND |
      XCB_GC_BACKGROUND |
      XCB_GC_GRAPHICS_EXPOSURES,  /* value mask */
      values                      /* value list */
    );
  }

  /* Create white graphics context. */
  xcb_gcontext_t white_gcontext = xcb_generate_id(connection);
  {
    uint32_t values[] = {
      screen->white_pixel,
      screen->black_pixel,
      0
    };
    xcb_create_gc(
      connection,                 /* connection */
      white_gcontext,             /* context id */
      screen->root,               /* drawable */
      XCB_GC_FOREGROUND |
      XCB_GC_BACKGROUND |
      XCB_GC_GRAPHICS_EXPOSURES,  /* value mask */
      values                      /* value list */
    );
  }

  /* Create window. */
  xcb_window_t window = xcb_generate_id(connection);
  {
    uint32_t values[] = {
      screen->white_pixel,
      XCB_EVENT_MASK_EXPOSURE |
      XCB_EVENT_MASK_KEY_PRESS |
      XCB_EVENT_MASK_POINTER_MOTION
    };
    xcb_create_window(
      connection,                             /* connection */
      screen->root_depth,                     /* depth */
      window,                                 /* window id */
      screen->root,                           /* parent */
      10,                                     /* x */
      10,                                     /* y */
      200,                                    /* width */
      200,                                    /* height */
      1,                                      /* border width */
      XCB_WINDOW_CLASS_INPUT_OUTPUT,          /* _class */
      screen->root_visual,                    /* visual */
      XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,  /* value mask */
      values                                  /* value list */
    );
  }

  /* Map (show) the window. */
  xcb_map_window(connection, window);

  /*
   * When the user attempts to close the window via the (X)
   * button drawn by the system's window manager the program
   * will recieve a XCB_CLIENT_MESSAGE event with the
   * WM_DELETE_WINDOW atom.
   */

  /* Get the WM_PROTOCOLS atom */
  xcb_atom_t wm_protocols =
    get_atom(connection, "WM_PROTOCOLS");

  /* Get the WM_DELETE_WINDOW atom */
  xcb_atom_t wm_delete_window =
    get_atom(connection, "WM_DELETE_WINDOW");

  /*
   * Change the WM_PROTOCOLS atom to use WM_DELETE_WINDOW.
   * Without this we don't get WM_DELETE_WINDOW messages.
   */
  xcb_change_property(
    connection,             /* connection */
    XCB_PROP_MODE_REPLACE,  /* mode */
    window,                 /* window */
    wm_protocols,           /* property */
    XCB_ATOM_ATOM,          /* type */
    sizeof(xcb_atom_t) * 8, /* format */
    1,                      /* data length */
    &wm_delete_window       /* data */
  );

  /* Message to draw in the window. */
  const char *title = "X11/XCB window example";

  /* Set the window title. */
  xcb_change_property(
    connection,             /* connection */
    XCB_PROP_MODE_REPLACE,  /* mode */
    window,                 /* window */
    XCB_ATOM_WM_NAME,       /* property */
    XCB_ATOM_STRING,        /* type */
    sizeof(char) * 8,       /* bits/element */
    strlen(title),          /* data length (element count) */
    title                   /* data */
  );

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

  /* TODO: convert image to screen->root_depth */

#ifdef USE_XCB_IMAGE
  xcb_image_t image;
  int create_image_result = create_image(
    &image,                     /* image */
    connection,                 /* connection */
    24,                         /* depth */
    XCB_IMAGE_FORMAT_Z_PIXMAP,  /* format */
    pixels,                     /* data */
    200,                        /* width */
    200,                        /* height */
    32                          /* bitmap padding */
  );

  if (create_image_result == 0)
  {
    fprintf(stderr, "Failed to create image\n");
    return 1;
  }
#endif

  xcb_pixmap_t pixmap = xcb_generate_id(connection);
  xcb_create_pixmap(
    connection, /* connection */
    24,         /* depth */
    pixmap,     /* pixmap id */
    window,     /* drawable */
    200,        /* width */
    200         /* height */
  );

#ifdef USE_XCB_IMAGE
  xcb_image_put(
    connection,     /* connection */
    pixmap,         /* drawable */
    black_gcontext, /* graphics context */
    &image,         /* image */
    0,              /* x */
    0,              /* y */
    0               /* left pad */
  );
#else
  xcb_put_image(
    connection,                 /* connection */
    XCB_IMAGE_FORMAT_Z_PIXMAP,  /* format */
    pixmap,                     /* drawable */
    black_gcontext,             /* graphics context */
    200,                        /* width */
    200,                        /* height */
    0,                          /* destination x */
    0,                          /* destination y */
    0,                          /* left pad */
    24,                         /* depth */
    200 * 200 * 4,              /* data length */
    pixels                      /* data */
  );
#endif

  /* Flush the commands to the X server. */
  xcb_flush(connection);

  /* Main event loop. */
  for (int running = 1; running;)
  {
    /* Get the next window event. */
    xcb_generic_event_t *event = xcb_wait_for_event(connection);

    switch (event->response_type & ~0x80)
    {
      case XCB_MOTION_NOTIFY: {
        xcb_motion_notify_event_t *e =
          (xcb_motion_notify_event_t*)event;

        xcb_rectangle_t rect;
        rect.x = rect.y = 0;
        rect.width = rect.height = 200;
        xcb_poly_fill_rectangle(
          connection,     /* connection */
          window,         /* drawable */
          black_gcontext, /* graphics context */
          1,              /* rectangles length */
          &rect           /* rectangles */
        );

        xcb_copy_area(
          connection,     /* connection */
          pixmap,         /* source drawable */
          window,         /* desintation drawable */
          black_gcontext, /* graphics context */
          0,              /* source x */
          0,              /* source y */
          e->event_x,     /* destination x */
          e->event_y,     /* destination y */
          200,            /* width */
          200             /* height */
        );

        xcb_flush(connection);
      } break;

      case XCB_EXPOSE: {
        xcb_rectangle_t rect;
        rect.x = rect.y = 0;
        rect.width = rect.height = 200;
        xcb_poly_fill_rectangle(
          connection,     /* connection */
          window,         /* drawable */
          black_gcontext, /* graphics context */
          1,              /* rectangles length */
          &rect           /* rectangles */
        );

        xcb_flush(connection);
      } break;

      case XCB_KEY_PRESS: {
        running = 0;
      } break;

      case XCB_CLIENT_MESSAGE: {
        xcb_client_message_event_t *e =
          (xcb_client_message_event_t*)event;

        if (e->data.data32[0] == wm_delete_window)
          running = 0;
      } break;
    }

    /* We must manually free events. */
    free(event);
  }

  /* Unmap (hide) the window. */
  xcb_unmap_window(connection, window);

  /* Free the pixmap. */
  xcb_free_pixmap(connection, pixmap);

  /* Free the graphics contexts. */
  xcb_free_gc(connection, black_gcontext);
  xcb_free_gc(connection, white_gcontext);

  /* Destroy the window. */
  xcb_destroy_window(connection, window);

  /* Flush commands to the X server. Not sure if needed. */
  xcb_flush(connection);

  /* Close the connection to the X server. */
  xcb_disconnect(connection);
}