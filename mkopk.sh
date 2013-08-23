rm -r build finite.opk
make -f Makefile.gcw0 clean
make -f Makefile.gcw0
mkdir build
mkdir build/res
cp finite build
cp icon.png build
cp default.gcw0.desktop build
cp -r res/*.png build/res
cp -r res/*.ogg build/res
cp -r res/*.wav build/res
cp -r res/*.txt build/res
mksquashfs build finite.opk -all-root -noappend -no-exports -no-xattrs
