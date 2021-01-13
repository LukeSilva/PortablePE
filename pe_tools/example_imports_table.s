.section .win_imports

.global _ExitProcess
.global _GetStdHandle
.global _WriteConsole


image_base = 0x00400000
windows_imports:
	.byte 'P', 'E', 0x00, 0x00
	.long _start_win
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

