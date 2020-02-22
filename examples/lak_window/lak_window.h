#ifndef LAK_WINDOW_H
#define LAK_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>

typedef struct _lak_window lak_window;
typedef lak_window* lak_window_t;

typedef void (*lak_blitter_t)(
  lak_window_t window,
  unsigned int pos_x,
  unsigned int pos_y,
  unsigned int width,
  unsigned int height,
  unsigned int vertical_stride_bytes,
  unsigned char *pixels);

typedef enum
{
  /* VVVVVVVV */
  lak_colour_8bit         = 1,

  /* RRRRRGGG GGGBBBBB */
  lak_colour_16bit_rgb_be = 4,

  /* BBBBBGGG GGGRRRRR */
  lak_colour_16bit_bgr_be = 5,

  /* GGGBBBBB RRRRRGGG */
  lak_colour_16bit_rgb_le = 4,

  /* GGGRRRRR BBBBBGGG */
  lak_colour_16bit_bgr_le = 5,

  /* RRRRRRRR GGGGGGGG BBBBBBBB */
  lak_colour_24bit_rgb    = 6,

  /* BBBBBBBB GGGGGGGG RRRRRRRR */
  lak_colour_24bit_bgr    = 7,

  /* RRRRRRRR GGGGGGGG BBBBBBBB  */
  lak_colour_24bit_rgbx   = 8,

  /* BBBBBBBB GGGGGGGG RRRRRRRR XXXXXXXX */
  lak_colour_24bit_bgrx   = 9,

  /* XXXXXXXX RRRRRRRR GGGGGGGG BBBBBBBB */
  lak_colour_24bit_xrgb   = 10,

  /* XXXXXXXX BBBBBBBB GGGGGGGG RRRRRRRR */
  lak_colour_24bit_xbgr   = 11,

  /* RRRRRRRR GGGGGGGG BBBBBBBB AAAAAAAA */
  lak_colour_32bit_rgba   = 12,

  /* BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA */
  lak_colour_32bit_bgra   = 13,

  /* AAAAAAAA RRRRRRRR GGGGGGGG BBBBBBBB */
  lak_colour_32bit_argb   = 14,

  /* AAAAAAAA BBBBBBBB GGGGGGGG RRRRRRRR */
  lak_colour_32bit_abgr   = 15
} lak_colour_type_t;

typedef struct
{
  unsigned char *data;
  unsigned long size;
  unsigned int width;
  unsigned int height;
  unsigned int vertical_stride_bytes;
  lak_colour_type_t type;
} lak_framebuffer_t;

void lak_blit(
  const lak_framebuffer_t* from,
  lak_framebuffer_t* to,
  unsigned int pos_x,
  unsigned int pos_y);

typedef struct lak_motion_event
{
  long x;
  long y;
} lak_motion_event_t;

typedef enum
{
  lak_event_type_nil      = 0,
  lak_event_type_expose   = 1,
  lak_event_type_keydown  = 2,
  lak_event_type_keyup    = 3,
  lak_event_type_motion   = 4,
  lak_event_type_close    = 5
} lak_event_type;

typedef struct lak_event
{
  lak_event_type type;
  union
  {
    lak_motion_event_t motion;
  };
} lak_event_t;

lak_window_t lak_create_window(
  int pos_x,
  int pos_y,
  unsigned int width,
  unsigned int height);

void lak_show_window(lak_window_t window);

void lak_set_title(lak_window_t window, const char *title);

void lak_set_wtitle(lak_window_t window, const wchar_t *title);

int lak_get_event(lak_window_t window, lak_event_t *event);

void lak_destroy_window(lak_window_t window);

#ifdef __cplusplus
}
#endif

#endif