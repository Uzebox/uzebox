rem ******************************************************
rem Compile the Telnet client CP/M program then add 
rem the roms to the disk image
rem *******************************************************

tasm ..\data\cpmsrc\telnet.asm -olist telnet.lst -o telnet.p
if %errorlevel% neq 0 exit /b %errorlevel%
p2bin telnet.p TELNET.COM
copy TELNET.COM ..\data\cpmbin
cpmfs ..\data\diskimg\CPMDISK0.DSK w TELNET.COM
cpmfs ..\data\diskimg\CPMDISK0.DSK w ..\data\cpmbin\TELNET.DAT
copy  ..\data\diskimg\CPMDISK0.DSK
