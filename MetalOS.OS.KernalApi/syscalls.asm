.code

x64SysCall MACRO name, id
&name& PROC
	mov r10, rcx
	mov eax, id
	syscall
	ret
&name& ENDP
ENDM

; This needs to be kept consistent with SystemCall in MetalOS.Internal.h. Figure out how
x64SysCall <GetSystemInfo>, 1
x64SysCall <GetProcessInfo>, 2
x64SysCall <ExitProcess>, 3
x64SysCall <ExitThread>, 4
x64SysCall <CreateWindow>, 5
x64SysCall <WaitForMessages>, 6
x64SysCall <GetMessage>, 7
x64SysCall <CreateFile>, 8
x64SysCall <ReadFile>, 9
x64SysCall <SetFilePosition>, 10
x64SysCall <VirtualAlloc>, 11
x64SysCall <DebugPrint>, 12

end

