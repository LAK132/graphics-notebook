#include "xcb/xcb.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/*
 * Helper function for getting atoms.
 * Getting atoms requires memory managment, so by containing
 * it in this function we reduce the chance of leaking.
 */
xcb_atom_t get_atom(
  xcb_connection_t *connection,
  const char *name
)
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
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS
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

  /* Flush the commands to the X server. */
  xcb_flush(connection);

  /* Main event loop. */
  for (int running = 1; running;)
  {
    /* Get the next window event. */
    xcb_generic_event_t *event = xcb_wait_for_event(connection);

    switch (event->response_type & ~0x80)
    {
      case XCB_EXPOSE: {
        xcb_rectangle_t rect;
        rect.x = rect.y = 0;
        rect.width = rect.height = 200;
        xcb_poly_fill_rectangle(
          connection,     /* connection */
          window,         /* drawable */
          white_gcontext, /* graphics context */
          1,              /* rectangles length */
          &rect           /* rectangles */
        );

        rect.x = rect.y = 10;
        rect.width = rect.height = 180;
        xcb_poly_fill_rectangle(
          connection,     /* connection */
          window,         /* drawable */
          black_gcontext, /* graphics context */
          1,              /* rectangles length */
          &rect           /* rectangles */
        );

        rect.x = rect.y = 20;
        rect.width = rect.height = 160;
        xcb_poly_fill_rectangle(
          connection,     /* connection */
          window,         /* drawable */
          white_gcontext, /* graphics context */
          1,              /* rectangles length */
          &rect           /* rectangles */
        );

        xcb_image_text_8(
          connection,     /* connection */
          strlen(title),  /* string length */
          window,         /* drawable */
          black_gcontext, /* graphics context */
          30,             /* x */
          100,            /* y */
          title           /* string */
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