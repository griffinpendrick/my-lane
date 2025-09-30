@echo off 

if not exist C:\mylane\bin mkdir C:\mylane\bin 
pushd C:\mylane\bin 

set CompilerFlags= /Od /Oi /Zi /MD /GR- /EHsc /WX /W4 /wd4201 /wd4100 /wd4189 /wd4505 /wd4244 /wd4456 /wd4701 /wd4703 /wd4127 /wd4267
set LinkerFlags= /incremental:no /opt:ref user32.lib kernel32.lib shell32.lib 
set IncludePath= C:\opencv\build\include

cl %CompilerFlags% C:\mylane\src\main.cpp /I %IncludePath% /link /LIBPATH:C:\opencv\build\x64\vc16\lib opencv_world4120.lib %LinkerFlags% /OUT:mylane.exe 

popd