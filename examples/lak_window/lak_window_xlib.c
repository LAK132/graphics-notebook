#include "lak_window.h"
#include "../lak_memory/lak_memory.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct _lak_window
{
  Display *display;
  Atom wm_delete_window;
  int screen;
  Window window;
  lak_framebuffer_t framebuffer;
};

Display *lak_get_display(lak_window_t window)
{
  return window->display;
}

int lak_get_screen(lak_window_t window)
{
  return window->screen;
}

Window lak_get_window(lak_window_t window)
{
  return window->window;
}

void lak_blit(
  const lak_framebuffer_t* from,
  lak_framebuffer_t* to,
  unsigned int pos_x,
  unsigned int pos_y)
{

}

lak_window_t lak_create_window(
  int pos_x,
  int pos_y,
  unsigned int width,
  unsigned int height)
{
  lak_window_t window =
    (lak_window_t)lak_malloc(sizeof(lak_window));
  if (!window) return NULL;

  window->display = XOpenDisplay(NULL);
  if (!window->display)
  {
    lak_free(window);
    return NULL;
  }

  window->screen = DefaultScreen(window->display);

  window->window = XCreateSimpleWindow(
    window->display,  /* display */
    RootWindow(       /* parent */
      window->display,
      window->screen),
    pos_x,            /* x */
    pos_y,            /* y */
    width,            /* width */
    height,           /* height */
    1,                /* border width */
    BlackPixel(       /* border */
      window->display,
      window->screen),
    WhitePixel(       /* background */
      window->display,
      window->screen));

  window->wm_delete_window = XInternAtom(
    window->display, "WM_DELETE_WINDOW", 0);

  XSetWMProtocols(
    window->display,
    window->window,
    &window->wm_delete_window,
    1);

  XSelectInput(
    window->display,
    window->window,
    ExposureMask |
    KeyPressMask |
    KeyReleaseMask |
    PointerMotionMask);

  return window;
}

void lak_show_window(lak_window_t window)
{
  XMapWindow(window->display, window->window);
}

void lak_set_wtitle(lak_window_t window, const wchar_t *title)
{
  size_t len = 0;

  for (const wchar_t *iter = title;
       *iter != 0;
       ++iter, ++len);

  if (len == 0) return;

  wchar_t *text = (wchar_t *)lak_malloc(sizeof(wchar_t) * len);

  for (wchar_t *iter = text;
       (*iter = *title) != 0;
       ++iter, ++title);

  XTextProperty property;
  XwcTextListToTextProperty(
    window->display,
    &text,
    1,
    XStringStyle,
    &property
  );
  XSetWMName(window->display, window->window, &property);
  XFree(property.value);
  lak_free(text);
}

void lak_set_title(lak_window_t window, const char *title)
{
  XStoreName(window->display, window->window, title);
}

int lak_get_event(lak_window_t window, lak_event_t *event)
{
  XEvent xevent;
  if (XPending(window->display) < 1)
  {
    // If there's no messages, immediately return.
    event->type = lak_event_type_nil;
    return 1;
  }

  // TODO: We need to check if the window is correct.

  XNextEvent(window->display, &xevent);
  switch (xevent.type)
  {
    case KeyPress:
      event->type = lak_event_type_keydown;
      break;

    case KeyRelease:
      event->type = lak_event_type_keyup;
      break;

    case MotionNotify:
      event->type = lak_event_type_motion;
      event->motion.x = xevent.xmotion.x;
      event->motion.y = xevent.xmotion.y;
      break;

    case ClientMessage:
      if ((Atom)xevent.xclient.data.l[0] ==
            window->wm_delete_window)
      {
        event->type = lak_event_type_close;
      }
      else
      {
        event->type = lak_event_type_nil;
      }
      break;

    case Expose:
      event->type = lak_event_type_expose;
      break;

    default:
      event->type = lak_event_type_nil;
      break;
  }
  return 1;
}

void lak_destroy_window(lak_window_t window)
{
  XDestroyWindow(window->display, window->window);

  XCloseDisplay(window->display);

  lak_free(window);
}
