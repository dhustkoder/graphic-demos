cl /W0 /EHsc /Ox /I C:\SDKS\SDL2\include C:\SDKS\SDL2\lib\x64\SDL2.lib C:\SDKS\SDL2\lib\x64\SDL2main.lib  oop.cpp /link /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup


cl /W0 /EHsc /Ox /I C:\SDKS\SDL2\include /I C:\SDKS\GLEW\include C:\SDKS\GLEW\lib\Release\x64\glew32.lib  C:\SDKS\SDL2\lib\x64\SDL2.lib C:\SDKS\SDL2\lib\x64\SDL2main.lib  dod.c /link opengl32.lib /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup


