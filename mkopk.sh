rm -r dist finite.opk
make -f Makefile.gcw0 clean
make -f Makefile.gcw0
mkdir dist
mkdir dist/res
cp finite dist
cp icon.png dist
cp default.gcw0.desktop dist
cp -r res/*.png dist/res
cp -r res/*.wav dist/res
cp -r res/*.txt dist/res
mksquashfs dist finite.opk -all-root -noappend -no-exports -no-xattrs
