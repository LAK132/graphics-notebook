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

xcb_test.elf: xcb_test.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb -lxcb-image -lxcb-icccm

xcb_test: xcb_test.elf
	./$<

#
# Wayland
#

simple_wayland.elf: simple_wayland.c Makefile
	gcc -Wall $< -o $@ -lwayland-client

simple_wayland: simple_wayland.elf
	./$<

#
# Win32
#

# https://eggplant.pro/blog/writing-win32-applications-with-mingw-and-wine/

WIN32_DEFINES = -DUNICODE -D_UNICODE -DWIN32 -D_WINDOWS -D_WIN32_WINNT=0x501
WIN32_LINKOPT = -Wl,--subsystem,windows
WIN32_LIB = -lcomctl32
WIN32_EMU = -mwin32 -mwindows -municode -mthreads

win32-g++ = x86_64-w64-mingw32-g++ $(WIN32_LINKOPT) $(WIN32_LIB) $(WIN32_EMU) $(WIN32_DEFINES) -static-libgcc -static-libstdc++

simple_win32.exe: simple_win32.cpp Makefile
	$(win32-g++) -Wall $< -o $@

simple_win32: simple_win32.exe
	wine64 $<

win32_console.o: win32_console.cpp win32_console.hpp Makefile
	$(win32-g++) -Wall -c $< -o $@

blit_win32.exe: blit_win32.cpp win32_console.o Makefile
	$(win32-g++) -Wall $< win32_console.o -o $@

blit_win32: blit_win32.exe
	wine64 $<

#
# Utils
#

clean:
	rm *.elf
