@echo off

if [%1] == []	goto help

del /y bip.dat
dat -a -t BMP bip.dat %1
img2spr %2
goto end

:help
echo USAGE:
echo "makespr source_bitmap.[bmp|pcx|tga|lbm] output_sprite.spr"

:end
