1.) Fix everything - so much seems broken right now
	Working printf is a must - why isn't that working?
	Touching the page directory causes everything to freak out
	We need to be able to identity map the 1-2MB range as well as the 0-1MB range
2.) Multithreading - assuming all of the core foundation stuff was working right, this'd be easy
	Task - A page directory and a message queue (and a terminal)
	Thread - An ESP and a task pointer
	I'll treat the kernel as one big task with multiple threads (swapping between threads in
	the kernel address space).
	The Interrupt 8 handler causes us (still in the process we're switching out of) to jump
	into the kernel's task-switching code (it's mapped for everybody). We push all of our 
	registers onto the stack and save ESP. We then restore the ESP and page directory of the
	next thread (the latter only assuming it's a task-switch), restore registers, and 
	resume.
3.) Devices & Subsystems
	Virtual Filesystem
	Actual filesystem (i.e. TAR driver)
	ATA PIO driver
	PS/2 driver (constantly polling, saves into buffer that terminal can poll)
	Virtuaal terminals
	Multitasking
	Task switcher
	Executable loading
4.) Test programs
	Tictactoe (ported from BF)
	"Terminal" (has a counting clock thread like POSterm)


Memory map idea
	The kernel and its memory is identity mapped from 0-4MB
		This is bad for security, but it's not like we care about that anyway
	Heap data starts at 0x800000 and goes up
	Stack starts at 0xC00000 and goes down
		Any additional thread stacks will start at 1MB intervals above that
We'll need a syscall interrupt


A "driver" as far as the VFS is concerned is a filesystem & disk driver working in concert
There will be a process for each device, which has an address space consisting of both the BLDD and
FS driver loaded at fixed addresses.
We'll need methods to manipulate devnums - a path is devnum/path
