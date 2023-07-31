cd boot
./build.sh
cd ..
cat boot/boot.bin hd0.tar > os.bin
