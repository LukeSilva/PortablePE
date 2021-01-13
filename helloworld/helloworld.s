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
	call *WIN32_GetStdHandle

	push $0
	push $0
	push $len
	push $msg
	push %eax
	call *WIN32_WriteConsoleA

#	push $console_title
#	call *WIN32_SetConsoleTitleA
#	
	push $2000
	call *WIN32_Sleep

	call fibbonacci
  push %eax
	call *WIN32_ExitProcess

.data                           # section declaration
msg:
	.ascii    "Hello World from a Portable Executable!\nWe will calculate the 11th term of the fibonnaci sequence, and return it as the exit code.\n\nNote: This string only occurs once in the .exe file - it's shared between Linux and Windows!\n"   # our dear string
	len = . - msg                 # length of our dear string

console_title:
	.asciz "Portable Exe Hello World"
