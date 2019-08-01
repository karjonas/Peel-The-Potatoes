#!/bin/sh
rm -rf build
mkdir build
wget -nc https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
cd build
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
make -j `nproc --all` install DESTDIR=AppDir
cd ..
LD_LIBRARY_PATH=../cocos2d/external/linux-specific/fmod/prebuilt/64-bit ./linuxdeploy-x86_64.AppImage --appdir build/AppDir --output appimage -d peel-the-potatoes.desktop -i ../proj.android/app/res/mipmap-hdpi/ic_launcher.png
