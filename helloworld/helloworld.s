.text
.global _start              
.global _start_win

fibbonacci:
	movl $0, %eax
	movl $1, %ebx
	movl $11, %ecx
	_loop:
		add %eax, %ebx

		movl %ebx, %edx
		movl %eax, %ebx
		movl %edx, %eax

		sub $1, %ecx
		cmp $0, %ecx
		jne _loop
	ret

_start:                                # write our string to stdout
	movl    $len,%edx           # third argument: message length
	movl    $msg,%ecx           # second argument: pointer to message to write
	movl    $1,%ebx             # first argument: file handle (stdout)
	movl    $4,%eax             # system call number (sys_write)
	int     $0x80               # call kernel
															# and exit
	call fibbonacci
	movl    %eax,%ebx             # first argument: exit code
	movl    $1,%eax             # system call number (sys_exit)
	int     $0x80               # call kernel

_start_win:
	push $-11
	call *_GetStdHandle

	push $0
	push $0
	push $len
	push $msg
	push %eax
	call *_WriteConsole

	call fibbonacci
  push %eax
	call *_ExitProcess

.data                           # section declaration
msg:
	.ascii    "Hello World from a Portable Executable!\nWe will calculate the 11th term of the fibonnaci sequence, and return it as the exit code.\n\nNote: This string only occurs once in the .exe file - it's shared between Linux and Windows!\n"   # our dear string
	len = . - msg                 # length of our dear string


.section .win_imports
image_base = 0x00400000
windows_imports:
	/* dll1 - kernel32.dll */
	.long _dll1_int - image_base 		# INT
	.long 0													# Date time stamp
	.long 0													# Forwarder Chain
	.long _dll1_name - image_base		# DLL Name
	.long _dll1_iat - image_base		# IAT

	/* Null Entry */ 
	.long 0 # INT
	.long 0 # Datetime
	.long 0 # Forwarder
	.long 0 # DLL Name
	.long 0 # IAT


_dll1_int:
	.align 4
	.long _dll1_ExitProcess - image_base
	.long _dll1_GetStdHandle - image_base
	.long _dll1_WriteConsole - image_base
	.long 0

_dll1_iat:
	.align 4
_ExitProcess:
	.long _dll1_ExitProcess - image_base
_GetStdHandle:
	.long _dll1_GetStdHandle - image_base
_WriteConsole:
	.long _dll1_WriteConsole - image_base
	.long 0
	.align 2

_dll1_GetStdHandle:
	.short 0 # Hint (cant be bothered to fill)
	.asciz "GetStdHandle"
	.align 2

_dll1_WriteConsole:
	.short 0
	.asciz "WriteConsoleA"
	.align 2

_dll1_ExitProcess:
	.short 0
	.asciz "ExitProcess"
	.align 2

_dll1_name: 
	.asciz "kernel32.dll"

