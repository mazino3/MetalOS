#pragma once

#define def_x64_GPR_RW(name) \
	uint64_t __stdcall x64_Read ## name ## (); \
	void __stdcall x64_Write ## name ## (uint64_t value);

extern "C"
{
	//Omitted from intrinsics
	void __stdcall x64_ltr(uint16_t entry);
	uint64_t __stdcall x64_rflags();
	void __stdcall x64_sti();
	void __stdcall x64_cli();

	//Segment Registers RW
	uint16_t __stdcall x64_ReadCS();
	uint16_t __stdcall x64_ReadDS();
	void __stdcall x64_WriteDS(uint16_t segment_selector);
	uint16_t __stdcall x64_ReadES();
	void __stdcall x64_WriteES(uint16_t segment_selector);
	uint16_t __stdcall x64_ReadFS();
	void __stdcall x64_WriteFS(uint16_t segment_selector);
	uint16_t __stdcall x64_ReadGS();
	void __stdcall x64_WriteGS(uint16_t segment_selector);
	uint16_t __stdcall x64_ReadSS();
	void __stdcall x64_WriteSS(uint16_t segment_selector);

	//General Purpose Register (RW)
	def_x64_GPR_RW(Rax);
	def_x64_GPR_RW(Rcx);
	def_x64_GPR_RW(Rdx);
	def_x64_GPR_RW(Rbx);
	def_x64_GPR_RW(Rsi);
	def_x64_GPR_RW(Rdi);
	def_x64_GPR_RW(Rsp);
	def_x64_GPR_RW(Rbp);
	def_x64_GPR_RW(R8);
	def_x64_GPR_RW(R9);
	def_x64_GPR_RW(R10);
	def_x64_GPR_RW(R11);
	def_x64_GPR_RW(R12);
	def_x64_GPR_RW(R13);
	def_x64_GPR_RW(R14);
	def_x64_GPR_RW(R15);

	//Helpers
	void __stdcall x64_update_segments(uint16_t data_selector, uint16_t code_selector);
}

