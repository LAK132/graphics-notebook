simple_xlib.elf: simple_xlib.c Makefile
	gcc $< -o $@ -lX11

simple_xcb: simple_xcb.elf
	./$<

simple_xcb.elf: simple_xcb.c Makefile
	gcc $< -o $@ -lX11 -lxcb

simple_wayland: simple_wayland.elf
	./$<

simple_wayland.elf: simple_wayland.c Makefile
	gcc $< -o $@ -lwayland-client

simple_xlib: simple_xlib.elf
	./$<

clean:
	rm *.elf
