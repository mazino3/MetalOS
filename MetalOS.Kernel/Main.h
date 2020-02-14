#pragma once

#define GNU_EFI_SETJMP_H
#include <efi.h>
#include <LoaderParams.h>
#include <stdarg.h>
#include "Kernel.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define Assert(x) if (!(x)) { kernel.Bugcheck("File: " __FILE__, "Line: " STR(__LINE__),  #x); }
#define Fatal(x) kernel.Bugcheck("File: " __FILE__, "Line: " STR(__LINE__),  #x); 


extern "C" void main_thunk(LOADER_PARAMS* loader);
void main(LOADER_PARAMS* loader);

void* operator new(size_t n);
void operator delete(void* p);
void operator delete(void* p, size_t n);

extern "C" void Print(const char* format, ...);
extern "C" void VPrint(const char* format, va_list args);

extern Kernel kernel;
