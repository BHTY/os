cd boot
./build.sh
cd ..

cd kernel
./build.sh
cd ..

cd hdd
./build.sh
cd ..

cat boot/boot.bin hdd/hd.tar > os.bin
