simple_xlib.elf: simple_xlib.c Makefile
	gcc $< -o $@ -lX11 -ldl

simple_xcb.elf: simple_xcb.c Makefile
	gcc $< -o $@ -lX11 -lxcb -ldl

clean:
	rm *.elf
