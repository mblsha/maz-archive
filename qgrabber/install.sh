#!/bin/sh

if [ ! -f ./qgrabber ]; then
  echo "'qgrabber' doesn't exist. You need to compile QGrabber first. Go read INSTALL file."
  exit 1
fi

# Modify this line, if you want to install into another place
INSTDIR=/usr/local/qgrabber

echo [Installing QGrabber]

mkdir -p $INSTDIR

echo Copying program files to $INSTDIR
mkdir -p $INSTDIR/plugins
cp -r ./plugins/*.so $INSTDIR/plugins

cp ./qgrabber    $INSTDIR
cp ./Readme.html $INSTDIR
cp ./COPYING     $INSTDIR
cp ./ChangeLog   $INSTDIR

cp ./*.so*       $INSTDIR

echo Copying Images
mkdir -p $INSTDIR/images
cp -r ./images/*.png $INSTDIR/images

echo Copying Icons
mkdir -p $INSTDIR/icons/main
cp -r ./icons/main/*.png    $INSTDIR/icons/main
mkdir -p $INSTDIR/icons/object
cp -r ./icons/object/*.png  $INSTDIR/icons/object
mkdir -p $INSTDIR/icons/plugins
cp -r ./icons/plugins/*.png $INSTDIR/icons/plugins

mkdir -p /usr/local/bin
if [ -e /usr/local/bin/qgrabber ]; then
  rm /usr/local/bin/qgrabber
fi

mkdir -p /usr/local/lib
if [ -e /usr/local/lib/libqgrabber.so ]; then
  rm /usr/local/lib/libqgrabber.so
fi

ln -s $INSTDIR/libqgrabber.so.1 /usr/local/lib/libqgrabber.so.1

echo Putting a link in /usr/local/bin
ln -s $INSTDIR/qgrabber /usr/local/bin/qgrabber

echo [Finished]

