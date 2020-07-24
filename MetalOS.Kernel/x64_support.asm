.code

; https://docs.microsoft.com/en-us/cpp/build/x64-software-conventions?view=vs-2019
CONTEXT struct
	_r12 qword ?
	_r13 qword ?
	_r14 qword ?
	_r15 qword ?
	_rdi qword ?
	_rsi qword ?
	_rbx qword ?
	_rbp qword ?
	_rsp qword ?
	_rip qword ?
	_rflags qword ?
CONTEXT ends

.data

public x64_CONTEXT_SIZE
x64_CONTEXT_SIZE QWORD SIZEOF CONTEXT

.code

; Context save and restore
; void* context
x64_save_context proc
	mov [rcx + CONTEXT._r12], r12
	mov [rcx + CONTEXT._r13], r13
	mov [rcx + CONTEXT._r14], r14
	mov [rcx + CONTEXT._r15], r15
	mov [rcx + CONTEXT._rdi], rdi
	mov [rcx + CONTEXT._rsi], rsi
	mov [rcx + CONTEXT._rbx], rbx
	mov [rcx + CONTEXT._rbp], rbp
	;RFLAGS
	pushfq
	pop rax
	mov [rcx + CONTEXT._rflags], rax
	;Return Address
	pop rdx
	mov [rcx + CONTEXT._rip], rdx
	mov [rcx + CONTEXT._rsp], rsp
	;Return 0 from function
	xor rax, rax
	jmp rdx
x64_save_context endp

x64_load_context proc
	mov r12, [rcx + CONTEXT._r12]
	mov r13, [rcx + CONTEXT._r13]
	mov r14, [rcx + CONTEXT._r14]
	mov r15, [rcx + CONTEXT._r15]
	mov rdi, [rcx + CONTEXT._rdi]
	mov rsi, [rcx + CONTEXT._rsi]
	mov rbx, [rcx + CONTEXT._rbx]
	mov rbp, [rcx + CONTEXT._rbp]
	mov rsp, [rcx + CONTEXT._rsp]
	; Return 1 from function
	mov rax, 1
	;Restore flags right befor jump
	mov r8, [rcx + CONTEXT._rflags]
	mov r9, [rcx + CONTEXT._rip]
	push r8
	popfq
	jmp r9
x64_load_context endp

; int x64_start_user_thread(void* context, void* teb);
x64_start_user_thread proc
	; Load registers
	mov r12, [rcx + CONTEXT._r12]
	mov r13, [rcx + CONTEXT._r13]
	mov r14, [rcx + CONTEXT._r14]
	mov r15, [rcx + CONTEXT._r15]
	mov rdi, [rcx + CONTEXT._rdi]
	mov rsi, [rcx + CONTEXT._rsi]
	mov rbx, [rcx + CONTEXT._rbx]
	mov rbp, [rcx + CONTEXT._rbp]
	mov rsp, [rcx + CONTEXT._rsp]
	
	; RFlags
	mov r11, [rcx + CONTEXT._rflags]
	mov rcx, [rcx + CONTEXT._rip]
	wrgsbase rdx
	sysretq
x64_start_user_thread endp

; void* context, void* stack, void* entry
; RCX RDX R8
x64_init_context proc
	;Setup stack and entry
	mov [rcx + CONTEXT._rsp], rdx
	mov [rcx + CONTEXT._rip], r8
	;Setup flags with interrupts enabled (so its ready to go upon restore)
	pushfq
	pop rax
	or rax, 200h
	mov [rcx + CONTEXT._rflags], rax
	ret
x64_init_context endp

x64_ltr proc
	ltr cx
	ret
x64_ltr endp

x64_sti proc
	sti
	ret
x64_sti endp

x64_cli proc
	sti
	ret
x64_cli endp

x64_swapgs proc
	swapgs
	ret
x64_swapgs endp

; mov's into CS isn't allowed. Therefore
; https://stackoverflow.com/questions/34264752/change-gdt-and-update-cs-while-in-long-mode
; https://forum.osdev.org/viewtopic.php?f=1&t=29883&sid=b954100cc5c685faa4a7f958bad5291a&start=15
; http://os-adventure.blogspot.com/2014/12/local-gdt-and-idt.html?view=classic
; void x64_update_segments(UINT16 dataSelector - CX, UINT16 codeSelector - DX)
x64_update_segments PROC
	; Populate segments with data selector
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx

	; Populate CS by using ret-far
	movzx rdx, dx ; copy code selector and extend it to 64 bits
	lea rcx, exit
	push rdx
	push rcx
	retfq
exit:
	ret
x64_update_segments ENDP

; CS RW
x64_ReadCS PROC
	mov ax, cs
	ret
x64_ReadCS ENDP

; GS RW
x64_ReadGS PROC
	mov ax, gs
	ret
x64_ReadGS ENDP

x64_WriteGS PROC
	mov gs, cx
	ret
x64_WriteGS ENDP

; SS RW
x64_ReadSS PROC
	mov ax, ss
	ret
x64_ReadSS ENDP

x64_disable_interrupts PROC
	pushfq
	cli
	pop rax
	ret
x64_disable_interrupts ENDP

x64_restore_flags PROC
	push rcx
	popfq
	ret
x64_restore_flags ENDP

x64_pause PROC
	pause
	ret
x64_pause ENDP

END
