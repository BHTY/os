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


VFS Device Management Operations
- int fs_installdevice(FS_DRIVER drv)
	An FS_DRIVER structure includes pointers to executable data for both filesystem and BLD drivers
	as well as any "configuration" information necessary. It returns the device number.
- int fs_uninstalldevice(int index)

VFS File Management Operations
- int fs_rename(char* old_path, char* new_path)
- int fs_mkdir(char* path)
- int fs_delete(char* path)
- int fs_getfileinfo(char* path, FS_FILEINFO* fileinfo);
	What this tells you is if the file exists, whether its a file or folder, and its size
- int fs_extractfilename(char* path, char* name);

Directory contents operations
- FS_DIR fs_opendir(char* path);
- int fs_setdirentry(FS_DIR dir, int entry);
- int fs_getdirentry(FS_DIR dir);
- int fs_readdir(FS_DIR dir, FS_ENTRY* entry);
	Increments the directory pointer (or returns 0 if there's nothing left) and stores the next
	directory entry name in the pointer.

VFS Core File Operations
- FS_FILE fs_openfile(char* path, int mode)      	analagous to C fopen
- int fs_closefile(FS_FILE file)	         	analagous to C fclose
- int fs_putbytes(FS_FILE file, char* bytes, int len)	analagous to C fwrite
- int fs_getbytes(FS_FILE file, char* bytes, int len)	analagous to C fread
- int fs_getpos(FS_FILE file)			 	analagous to C ftell
- int fs_setpos(FS_FILE file, int pos, int offset)	analagous to C fseek 

What is an open file handle? (VFS perspective)
- An indication of the driver to which the file belongs 
- An index into the file handle table of the driver
- Mode

What is an open file handle? (driver perspective)
- The sector on which the file begins
- The file size
- The pointer to the current file item

The VFS state is internal to the kernel

What is the process for a filesystem call? Usermode libraries do a system call via an interrupt, which is caught by an interrupt handler which causes a kernel-mode transition, calling the relevant VFS function. The relevant VFS function then sends a message to the relevant driver process and waits for a return message to indicate that the operation is complete before returning.
On the side of the driver, the filesystem driver creates a thread to execute the operation. The thread waits until the device is ready (since another process may be trying to do its own work) and finally does the operation, then sends a message back to the caller.



Currently, there is only one filesystem driver available - TAR, with two block-level device drivers - RAMdisk and IDE/ATA PIO. BLDDs expose two primary functions
	int bldd_writesector(int sector, int sector_count, char* data);
	int bldd_readsector(int sector, int sector_count, char* data);

