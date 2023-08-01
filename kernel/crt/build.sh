i386-elf-gcc -ffreestanding -m32 -g -c "string.c" -o "string.o"
i386-elf-gcc -ffreestanding -m32 -g -c "printf.c" -o "printf.o"
i386-elf-ld -o "crt.o" "string.o" "printf.o"
