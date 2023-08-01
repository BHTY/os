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

Currently, there is only one filesystem driver available - TAR, with two block-level device drivers - RAMdisk and IDE/ATA PIO. BLDDs expose two primary functions
	int bldd_writesector(int sector, int sector_count, char* data);
	int bldd_readsector(int sector, int sector_count, char* data);


ANATOMY OF A FILESYSTEM CALL
Usermode libraries eventually issue a system call via an interrupt to perform a VFS operation,
which is caught by the syscall interrupt handler in the kernel, which calls the relevant VFS 
function. The relevant VFS function then sends a message to the relevant driver process for the
requested device and waits for a return message indicating completion of the operation before
returning control to the caller.

On the side of the driver, the filesystem driver creates a thread to execute the operation. The 
thread waits until the device is ready (since another process may be trying to do its own work)
and finally does the operation, then sends a message back to the caller (which is in the VFS code
waiting for a response back).



TASK SWITCHING
The time-slice scheduler periodically gives execution time to all threads currently running on the
machine, but only one "foreground" process may be active at a time. The foreground process is the
one that has access to the terminal.
Each application is provided a virtual terminal with its own virtual screen and input buffers.
When the system call is issued to perform a terminal operation, the kernel first performs the 
operation on the virtual terminal, and then to the relevant terminal emulator for the machine.
Background applications are connected to the "null emulator", which reads no input and provides no
output, while the console emulator is connected to the actual display and reads input.

The computer's PS/2 driver is constantly running as its own process, polling the keyboard for any
additional keyboard events. Most of the time, it just stores keyboard events into a buffer to be
read by the console driver, which in turn sends them to the foreground application. However, the
kernel intercepts certain keypresses, what are known as "system keys".

With respect to buffering, there are multiple levels
- The PS/2 driver's own buffering
- The console driver's buffering
- The virtual terminal's buffering

If an application has not processed all of the keys in its virtual terminal's buffer, there may
still be inputs left in the queue after a task-switch, though they will only be keys pressed or
released before the task-switch. Note that this means that if a key is pushed but not released
before a task-switch, the application will act as if the key is continuing to be held down until
the application is resumed, the key is pushed down again, and then released.

There are three main system keys related to task switching, that serve to perform three operations.
- Switch to the next task
- Open the Task Manager
- Kill/exit the current task (this would be similar to the application calling exit())

The kernel has a system call that serves to change the task that is connected to the console
driver (essentially overwriting an internal variable inside of the kernel's terminal system) and
pressing the key combination to switch to the next task performs this. When the call is answered,
the first thing that is done by the console driver is copying the virtual screen buffer from the
newly-foregrounded application to the physical screen buffer.

Alternatively, a user could press a different key combination in order to open the Task Manager.
The Task Manager temporarily takes over the console driver, and provides a list of applications
(along with their current number of pages in use) that you can switch to. Alternatively, you can
also force-quit an application from this menu, or even start a new application. Note that the Task
Manager is essentially a process of its own, though it is also part of the kernel. You can switch
out of the Task Manager, but it will then automatically close itself.

There is also a dedicated key combination to force-quit an application. This performs the same
action as killing it through the Task Manager. Killing an application performs a few main actions
- Freeing all pages allocated by the application
- Freeing its page directory and page tables
- Deleting all associated threads

DEBUGGER
Breakpoints
Pausing execution
Single stepping
Register & stack dumps


THE HEAP & THE STACK
The heap and the stack (along with data embedded in the executable's data segment) are the two 
main areas in which applications store information. The stack stores variables local to the
execution frame / function, along with arguments and parameters to subroutines, register contents
on a task switch, return addresses, et cetera. 

When a task is created, the first item to occur is the creation of a task_t structure in the
kernel's scheduler. Tasks are stored in a doubly-linked list on the system, and the new structure
is inserted. A new virtual terminal is created, earmarked for the use of the new process (though
this too is an internal kernel data structure, accessed indirectly through syscalls that use kernel
code to route to the correct vterm), a message queue is allocated, and a page is allocated to store
the page directory.

After this "process creation" phase, process initialization is then performed, which primarily
consists of filling out the memory map for the process. First of all, core kernel data is identity
mapped. A single page is allocated for the process heap, and then the executable binary data is 
mapped into the process address space and copied in.

Finally, a thread is created, assigned the task ID of the new task, a stack page is allocated and
stored in ESP, with EIP ready to be restored, pointing to the _start() function. Every thread has
the thread_cleanup() function at the back of the stack, which serves to eject the thread from the
list and deallocate its stack when a thread returns, but _start() is supposed to circumvent this.

void _start(){
	// ... initialization
	main();
	exit();
}

_start() initializes the parent thread, which calls main(). When main() returns, exit() should kill
the process.

When a thread tries to access data in a page below its stack which is not allocated, a page fault
will be caused, and the stack will then grow with the process allocating an additional page.

The heap uses a linear memory block which will extend the heap as it grows by allocating more 
pages, but freeing pages is a more complicated issue.
