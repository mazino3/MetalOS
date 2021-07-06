#pragma once
#include <cstdint>

//Shared header between loader and kernel

enum UserAddress : uintptr_t
{
	UserStart = 0x0,
	UserStop = 0x0000800000000000,
};

const size_t KernelSectionLength = 0x10'0000'0000; //64GB
enum KernelAddress : uintptr_t
{
	KernelStart = 0xFFFF'8000'0000'0000,

	//Kernel libraries
	KernelBaseAddress = 0xFFFF'8000'0100'0000,
	KernelKdComBaseAddress = 0xFFFF'8000'0200'0000,

	//Kernel pdbs
	KernelKernelPdb = 0xFFFF'8000'1100'0000,
	KernelKdComPdb = 0xFFFF'8000'1200'0000,

	//Hardware 0xFFFF'8010'0000'0000
	KernelHardwareStart = KernelStart + KernelSectionLength,
	KernelPageTablesPool = KernelHardwareStart, //32MB (512 pages used)
	KernelGraphicsDevice = KernelHardwareStart + 0x200'0000, //32MB
	KernelRamDrive = KernelHardwareStart + 0x400'0000, //64MB
	KernelHardwareEnd = KernelHardwareStart + 0x800'0000,
	KernelPfnDbStart = KernelHardwareStart + 0x1'0000'0000,//4GB

	//Heap 0xFFFF'8020'0000'0000
	KernelHeapStart = KernelHardwareStart + KernelSectionLength,
	KernelHeapEnd = KernelHeapStart + KernelSectionLength,

	//UEFI 0xFFFF'8030'0000'0000
	KernelUefiStart = KernelHeapEnd,
	KernelUefiEnd = KernelUefiStart + KernelSectionLength,

	//ACPI 0xFFFF'8040'0000'0000
	KernelAcpiStart = KernelUefiEnd,
	KernelAcpiEnd = KernelAcpiStart + KernelSectionLength,

	//IO 0xFFFF'8050'0000'0000 (physical page)
	KernelIoStart = KernelAcpiEnd,
	KernelIoEnd = KernelIoStart + KernelSectionLength,

	KernelEnd = 0xFFFF'FFFF'FFFF'FFFF
};
const size_t RamDriveSize = KernelHardwareEnd - KernelRamDrive;

#define PAGE_SHIFT  12
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define PAGE_MASK   0xFFF

#define SIZE_TO_PAGES(a)  \
    ( ((a) >> PAGE_SHIFT) + ((a) & PAGE_MASK ? 1 : 0) )

#define MemoryMapReservedSize PAGE_SIZE

//4mb reserved space
#define BootloaderPagePoolCount 256
#define ReservedPageTablePages 512

#define KERNEL_GLOBAL_ALIGN __declspec(align(64))
#define KERNEL_PAGE_ALIGN __declspec(align(PAGE_SIZE))

#define QWordHigh(x) (((uint64_t)x) >> 32)
#define QWordLow(x) ((uint32_t)((uint64_t)x))

typedef size_t cpu_flags_t;

enum class Result
{
	Success,
	Failed,
	NotImplemented
};

//Keep in sync with MetalOS.KernalApi syscalls.asm
enum class SystemCall : size_t
{
	GetSystemInfo = 0x100,
	GetTickCount,

	GetCurrentThread = 0x200,
	CreateThread,
	Sleep,
	SwitchToThread,
	SuspendThread,
	ResumeThread,
	ExitProcess,
	ExitThread,

	CreateWindow = 0x300,
	GetWindowRect,
	GetMessage,
	PeekMessage,
	SetScreenBuffer,

	CreateFile = 0x400,
	ReadFile,
	WriteFile,
	SetFilePointer,
	CloseFile,
	MoveFile,
	DeleteFile,
	CreateDirectory,

	VirtualAlloc = 0x500,

	DebugPrint = 0x600,
};

