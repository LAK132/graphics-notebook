#
# Xlib
#

simple_xlib.elf: simple_xlib.c Makefile
	gcc -Wall $< -o $@ -lX11

simple_xlib: simple_xlib.elf
	./$<

blit_xlib.elf: blit_xlib.c Makefile
	gcc -Wall $< -o $@ -lX11

blit_xlib: blit_xlib.elf
	./$<

static_blit_xlib.elf: blit_xlib.c Makefile
	gcc -Wall $< -o $@ -lX11 -DSTATIC_IMAGE_BLIT

static_blit_xlib: blit_xlib.elf
	./$<

#
# XCB
#

simple_xcb.elf: simple_xcb.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb

simple_xcb: simple_xcb.elf
	./$<

blit_direct_xcb.elf: blit_direct_xcb.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb

blit_direct_xcb: blit_direct_xcb.elf
	./$<

blit_pixmap_xcb.elf: blit_pixmap_xcb.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb

blit_pixmap_xcb: blit_pixmap_xcb.elf
	./$<

blit_pixmap_image_xcb.elf: blit_pixmap_xcb.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb -lxcb-image -DUSE_XCB_IMAGE

blit_pixmap_image_xcb: blit_pixmap_image_xcb.elf
	./$<

#
# Wayland
#

simple_wayland.elf: simple_wayland.c Makefile
	gcc -Wall $< -o $@ -lwayland-client

simple_wayland: simple_wayland.elf
	./$<

#
# Utils
#

clean:
	rm *.elf
