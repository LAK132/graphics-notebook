if "%program%"=="simple_win32.exe" (
    call %CXX% /Fe:simple_win32.exe simple_win32.cpp User32.lib Gdi32.lib %LINK_ARGS% /SUBSYSTEM:WINDOWS
    goto :eof
)

echo Unknown program "%program%"