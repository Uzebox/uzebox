rem ******************************************************
rem Compile the netstat CP/M program then add 
rem the roms to the disk image
rem *******************************************************

tasm ..\data\cpmsrc\netstat.asm -olist netstat.lst -o netstat.p
if %errorlevel% neq 0 exit /b %errorlevel%
p2bin netstat.p NET.COM
if %errorlevel% neq 0 exit /b %errorlevel%
copy NET.COM ..\data\cpmbin
cpmfs ..\data\diskimg\CPMDISK0.DSK w NET.COM
if %errorlevel% neq 0 exit /b %errorlevel%
copy  ..\data\diskimg\CPMDISK0.DSK
