#include "Main.h"

#include "msvc.h"
#include <efi.h>
#include <LoaderParams.h>
#include <crt_stdio.h>
#include <vector>
#include <intrin.h>
#include "MetalOS.Kernel.h"
#include "MetalOS.h"
#include "System.h"
#include "MemoryMap.h"
#include "PageTables.h"
#include "PageTablesPool.h"
#include "x64.h"
#include "KernelHeap.h"
#include "Bitvector.h"
extern "C"
{
#include <acpi.h>
}
#include "x64_support.h"
#include "BootHeap.h"

//The one and only
Kernel kernel;

//Kernel Stack - set by x64_main
KERNEL_PAGE_ALIGN volatile UINT8 KERNEL_STACK[KERNEL_STACK_SIZE] = { 0 };
extern "C" UINT64 KERNEL_STACK_STOP = (UINT64)&KERNEL_STACK[KERNEL_STACK_SIZE];

//Syscall stack
KERNEL_PAGE_ALIGN volatile UINT8 SYSCALL_STACK[SYSCALL_STACK_SIZE] = { 0 };
extern "C" UINT64 SYSCALL_STACK_STOP = (UINT64)&SYSCALL_STACK[SYSCALL_STACK_SIZE];

//Boot Heap
KERNEL_PAGE_ALIGN static volatile UINT8 BOOT_HEAP[BOOT_HEAP_SIZE] = { 0 };
KERNEL_GLOBAL_ALIGN BootHeap bootHeap((void*)BOOT_HEAP, BOOT_HEAP_SIZE);

extern "C" void INTERRUPT_HANDLER(InterruptVector vector, PINTERRUPT_FRAME pFrame)
{
	kernel.HandleInterrupt(vector, pFrame);
}

void Print(const char* format, ...)
{
	va_list args;

	va_start(args, format);
	kernel.Printf(format, args);
	va_end(args);
}

void Print(const char* format, va_list args)
{
	kernel.Printf(format, args);
}

void* operator new(size_t n)
{
	uintptr_t callerAddress = (uintptr_t)_ReturnAddress();
	if (kernel.IsHeapInitialized())
		return kernel.Allocate(n, callerAddress);
	else
		return (void*)bootHeap.Allocate(n);
}

void operator delete(void* p)
{
	if (kernel.IsHeapInitialized())
		kernel.Deallocate(p);
	else
		bootHeap.Deallocate(p);
}

void operator delete(void* p, size_t n)
{
	if (kernel.IsHeapInitialized())
		kernel.Deallocate(p);
	else
		bootHeap.Deallocate(p);
}

void* malloc(size_t size)
{
	uintptr_t callerAddress = (uintptr_t)_ReturnAddress();
	Assert(kernel.IsHeapInitialized());
	return kernel.Allocate(size, callerAddress);
}

void free(void* ptr)
{
	Assert(kernel.IsHeapInitialized());
	return kernel.Deallocate(ptr);
}

extern "C" uint64_t SYSTEMCALL_HANDLER(SystemcallFrame* frame)
{
	return kernel.Syscall(frame);
}

void __declspec(noreturn) Bugcheck(const char* file, const char* line, const char* assert)
{
	kernel.Bugcheck(file, line, assert);
}

void main(LOADER_PARAMS* loader)
{
	//Initialize platform
	x64::Initialize();

	//Initialize kernel
	kernel.Initialize(loader);

	//Load shell
	kernel.CreateProcess("shell.exe");

	//Should never get here
	while (true)
		__halt();
}
