cd crt
./build.sh
cd ..

cd heap
./build.sh
cd ..

cd term
./build.sh
cd ..

i386-elf-gcc -O0 -ffreestanding -m32 -g -c "kernel.c" -o "kernel.o"
i386-elf-gcc -O0 -ffreestanding -m32 -g -c "idt.c" -o "idt.o"
i386-elf-gcc -O0 -ffreestanding -m32 -g -c "paging.c" -o "paging.o"
i386-elf-gcc -O0 -ffreestanding -m32 -g -c "io.c" -o "io.o"
nasm "paging.asm" -f elf -o "paging_asm.o"
nasm "idt.asm" -f elf -o "idt_asm.o"
nasm "kernel_entry.asm" -f elf -o "kernel_entry.o"
i386-elf-ld -o "kernel.bin" -Ttext 0x1000 "kernel_entry.o" "io.o" "heap/heap.o" "crt/crt.o" "kernel.o" "paging.o" "paging_asm.o" "idt.o" "idt_asm.o" "term/term.o" --oformat binary
