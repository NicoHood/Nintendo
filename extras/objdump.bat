@ echo off
REM Program to output the elf file from an Arduino sketch
title Get objdump from elf

echo Please enter the path of the temp .cpp.elf file
set /p elfpath=
echo.

set idepath=C:\Users\Nico\Documents\arduino-1.6.3
set destpath=C:\Arduino.txt
set command=%idepath%\hardware\tools\avr\bin\avr-objdump -S %elfpath% ^> %destpath%

cd %idepath%

:disassemble
echo %command%
echo.
%command%
echo.

pause

goto disassemble