nasm "boot.asm" -f bin -o "boot10.bin"
nasm "boot15.asm" -f bin -o "boot15.bin"
cat boot10.bin boot15.bin > boot.bin
