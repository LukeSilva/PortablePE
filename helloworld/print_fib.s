.text
.global _start              
.global _start_win

strlen:
	movl %eax, %ebx
	_psl:
		cmpb $0,(%ebx)
		jz _psle
		inc %ebx
		jmp _psl
	_psle:
	subl %eax, %ebx
	movl %ebx, %eax
	ret

linux_print_string:
	push %eax # Make a copy of string address
	call strlen # Get length into %edx
	movl %eax, %edx
	pop %ecx # String into %ecx
	movl $1, %ebx # stdout
	movl $4, %eax # write
	int $0x80 #syscall
	ret

win_print_string:
	push %eax
	call strlen
	push %eax
	push $-11
	call *WIN32_GetStdHandle
	pop %ecx # len
	pop %ebx # msg

	push $0
	push $0
	push %ecx #len
	push %ebx #msg
	push %eax #std handle
	call *WIN32_WriteConsoleA
	ret

linux_exit:
	movl    %eax,%ebx             # first argument: exit code
	movl    $1,%eax             # system call number (sys_exit)
	int     $0x80               # call kernel

win_exit:
  push %eax
	call *WIN32_ExitProcess

lib_print_int:

	movl $0, %ecx
	_pidl:
		inc %ecx
		movl $0, %edx
		movl $10, %ebx
		idivl %ebx
		addl $48, %edx
		pushl %edx
		cmp $0, %eax
		jnz _pidl
	_pipl:
		dec %ecx
		pop %eax
		movb %eax, number
		movl $number, %eax

		push %ecx
		call *lib_print_string
		pop %ecx

		cmpl $0, %ecx
		jnz _pipl
	ret

fibbonacci:
	movl $0, %eax
	movl $1, %ebx
	movl $11, %ecx
	_loop:
		add %eax, %ebx

		movl %ebx, %edx
		movl %eax, %ebx
		movl %edx, %eax

		push %eax
		push %ebx
		push %ecx
		push %edx
		call lib_print_int
		movl $nl, %eax
		call *lib_print_string
		pop %edx
		pop %ecx
		pop %ebx
		pop %eax

		sub $1, %ecx
		cmp $0, %ecx
		jne _loop
	ret

_start:                                # write our string to stdout
	movl $msg, %eax
	call *lib_print_string
	#call lib_print_int
	call fibbonacci
	call *lib_exit

_start_win:
	movl $win_print_string, %eax
	movl %eax, lib_print_string
	movl $win_exit, %eax
	movl %eax, lib_exit
	jmp _start
	
.data                           # section declaration

lib_print_string: .long linux_print_string
lib_exit: .long linux_exit
msg:
	.asciz    "Hello World from Portable PE!\nGenerating Fibonnacci:\n"   # our dear string

#nl:
	.asciz "\n"
number:
	.asciz "0"
	
nl:
	.asciz "\n"
console_title:
	.asciz "Portable Exe Hello World"
