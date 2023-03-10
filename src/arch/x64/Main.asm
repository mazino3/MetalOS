INCLUDE MetalOS.Kernel.s
INCLUDE x64.s

.CODE

; void ArchMain();
ArchMain PROC
	; Setup stack
	mov rsp, KERNEL_STACK_STOP ; Stack grows from high to low
	sub rsp, StackReserve ; Register parameter area

	; Jump to main
	jmp main
ArchMain ENDP

END

