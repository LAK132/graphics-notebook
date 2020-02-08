simple_xlib.elf: simple_xlib.c Makefile
	gcc $< -o $@ -lX11

simple_xlib: simple_xlib.elf
	./$<

blit_xlib.elf: blit_xlib.c Makefile
	gcc $< -o $@ -lX11

blit_xlib: blit_xlib.elf
	./$<

static_blit_xlib.elf: blit_xlib.c Makefile
	gcc $< -o $@ -lX11 -DSTATIC_IMAGE_BLIT

static_blit_xlib: blit_xlib.elf
	./$<

simple_xcb.elf: simple_xcb.c Makefile
	gcc $< -o $@ -lX11 -lxcb

simple_xcb: simple_xcb.elf
	./$<

simple_wayland.elf: simple_wayland.c Makefile
	gcc $< -o $@ -lwayland-client

simple_wayland: simple_wayland.elf
	./$<

clean:
	rm *.elf
