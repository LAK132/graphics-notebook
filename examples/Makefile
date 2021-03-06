#
# Xlib
#

simple/xlib.elf: simple/xlib.c Makefile
	gcc -Wall $< -o $@ -lX11

simple_xlib: simple/xlib.elf
	./$<

blit/xlib.elf: blit/xlib.c Makefile
	gcc -Wall $< -o $@ -lX11

blit_xlib: blit/xlib.elf
	./$<

blit/static_xlib.elf: blit/xlib.c Makefile
	gcc -Wall $< -o $@ -lX11 -DSTATIC_IMAGE_BLIT

blit_static_xlib: blit/xlib.elf
	./$<

lak_window_xlib.o: lak_window/lak_window_xlib.c Makefile
	gcc -Wall -c $< -o $@ -lX11

lak_memory_linux.o: lak_memory/lak_memory_crt.c Makefile
	gcc -Wall -c $< -o $@

library/xlib.elf: library/xlib.c lak_window_xlib.o lak_memory_linux.o Makefile
	gcc -Wall $< lak_window_xlib.o lak_memory_linux.o -o $@ -lX11

library_xlib: library/xlib.elf
	./$<

#
# XCB
#

simple/xcb.elf: simple/xcb.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb

simple_xcb: simple/xcb.elf
	./$<

blit/direct_xcb.elf: blit/direct_xcb.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb

blit_direct_xcb: blit/direct_xcb.elf
	./$<

blit/pixmap_xcb.elf: blit/pixmap_xcb.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb

blit_pixmap_xcb: blit/pixmap_xcb.elf
	./$<

blit/pixmap_image_xcb.elf: blit/pixmap_xcb.c Makefile
	gcc -Wall $< -o $@ -lX11 -lxcb -lxcb-image -DUSE_XCB_IMAGE

blit_pixmap_image_xcb: blit/pixmap_image_xcb.elf
	./$<

#
# Wayland
#

simple/wayland.elf: simple/wayland.c Makefile
	gcc -Wall $< -o $@ -lwayland-client

simple_wayland: simple/wayland.elf
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

simple/win32.exe: simple/win32.cpp Makefile
	$(win32-g++) -Wall $< -o $@

simple_win32: simple/win32.exe
	wine64 $<

lak_window_win32.o: lak_window/lak_window_win32.cpp Makefile
	$(win32-g++) -Wall -c $< -o $@

lak_memory_windows.o: lak_memory/lak_memory_crt.c Makefile
	$(win32-g++) -Wall -c $< -o $@

library/win32.exe: library/win32.cpp lak_window_win32.o lak_memory_windows.o Makefile
	$(win32-g++) -Wall $< lak_window_win32.o lak_memory_windows.o -o $@

library_win32: library/win32.exe
	wine64 $<

win32_console.o: win32_console.cpp win32_console.hpp Makefile
	$(win32-g++) -Wall -c $< -o $@

blit/win32.exe: blit/win32.cpp win32_console.o Makefile
	$(win32-g++) -Wall $< win32_console.o -o $@

blit_win32: blit/win32.exe
	wine64 $<

#
# Utils
#

clean:
	rm -f $(foreach dir,. simple blit lak_window lak_memory,$(dir)/*.elf $(dir)/*.exe $(dir)/*.o)
