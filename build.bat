@echo off

if not exist "build" mkdir build

pushd build
call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

set CommonCompilerFlags=-Od -EHsc -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC


REM cl  %CommonCompilerFlags% ../main.cpp ../url_parser.c ../tinyxml2.cpp /link Wininet.lib Urlmon.lib OleAut32.lib
REM cl  %CommonCompilerFlags% ../win.c /link Wininet.lib Urlmon.lib OleAut32.lib User32.lib Gdi32.lib Ole32.lib
REM cl -DUNICODE=1 /FC /EHsc /nologo ../win.cpp ../dll.c ../tinyxml2.cpp /link Wininet.lib Urlmon.lib OleAut32.lib User32.lib Gdi32.lib Ole32.lib
REM cl -DUNICODE /Zi /FC /EHsc /nologo ../listbox.cpp ../tinyxml2.cpp /link Wininet.lib Urlmon.lib OleAut32.lib User32.lib Gdi32.lib Ole32.lib
cl -DUNICODE /Zi /FC /EHsc /nologo ../Splitter_Wnd.cpp ../dll.c ../tinyxml2.cpp ../WndProc.cpp /link Wininet.lib Urlmon.lib OleAut32.lib User32.lib Gdi32.lib Ole32.lib ComCtl32.lib


echo Done

popd
