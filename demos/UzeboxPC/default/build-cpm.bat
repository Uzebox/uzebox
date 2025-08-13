rem ******************************************************
rem Compile the CPM/2 2.2 source and generate and include file. 
rem Also copies the disk images to the default folder to 
rem simplify work with the emulator.
rem *******************************************************

tasm ../data/cpmsrc/cpm22.asm -olist cpm22.lst -o cpm22.p
@if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%

p2bin cpm22.p
@if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%

bin2inc cpm22.bin >cpm22.inc
@if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%

move cpm22.inc ../data
@if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%

copy ..\data\diskimg\CPMDISK0.DSK
@if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%

copy ..\data\diskimg\CPMDISK1.DSK
@if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%

copy ..\data\diskimg\CPMDISK2.DSK
@if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL% 