rem ******************************************************
rem Compile the Uzenet config app CP/M program then add 
rem the rom to the disk image
rem *******************************************************

tasm ..\data\cpmsrc\uzeconf.asm -olist uzeconf.lst -o uzeconf.p
if %errorlevel% neq 0 exit /b %errorlevel%
p2bin uzeconf.p UZECONF.COM
if %errorlevel% neq 0 exit /b %errorlevel%
copy UZECONF.COM ..\data\cpmbin
cpmfs ..\data\diskimg\CPMDISK0.DSK w UZECONF.COM
if %errorlevel% neq 0 exit /b %errorlevel%
copy  ..\data\diskimg\CPMDISK0.DSK

 