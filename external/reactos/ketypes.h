#pragma once

//
// Debug Filter Levels
//
#define DPFLTR_ERROR_LEVEL                  0
#define DPFLTR_WARNING_LEVEL                1
#define DPFLTR_TRACE_LEVEL                  2
#define DPFLTR_INFO_LEVEL                   3
#define DPFLTR_MASK                         0x80000000

//
// Debug Status Codes
//
#define DBG_STATUS_CONTROL_C                1
#define DBG_STATUS_SYSRQ                    2
#define DBG_STATUS_BUGCHECK_FIRST           3
#define DBG_STATUS_BUGCHECK_SECOND          4
#define DBG_STATUS_FATAL                    5
#define DBG_STATUS_DEBUG_CONTROL            6
#define DBG_STATUS_WORKER                   7

//
// DebugService Control Types
//
#define BREAKPOINT_BREAK                    0
#define BREAKPOINT_PRINT                    1
#define BREAKPOINT_PROMPT                   2
#define BREAKPOINT_LOAD_SYMBOLS             3
#define BREAKPOINT_UNLOAD_SYMBOLS           4
#define BREAKPOINT_COMMAND_STRING           5

//
// Debug Control Codes for NtSystemDebugcontrol
//
typedef enum _SYSDBG_COMMAND
{
	SysDbgQueryModuleInformation = 0,
	SysDbgQueryTraceInformation = 1,
	SysDbgSetTracepoint = 2,
	SysDbgSetSpecialCall = 3,
	SysDbgClearSpecialCalls = 4,
	SysDbgQuerySpecialCalls = 5,
	SysDbgBreakPoint = 6,
	SysDbgQueryVersion = 7,
	SysDbgReadVirtual = 8,
	SysDbgWriteVirtual = 9,
	SysDbgReadPhysical = 10,
	SysDbgWritePhysical = 11,
	SysDbgReadControlSpace = 12,
	SysDbgWriteControlSpace = 13,
	SysDbgReadIoSpace = 14,
	SysDbgWriteIoSpace = 15,
	SysDbgReadMsr = 16,
	SysDbgWriteMsr = 17,
	SysDbgReadBusData = 18,
	SysDbgWriteBusData = 19,
	SysDbgCheckLowMemory = 20,
	SysDbgEnableKernelDebugger = 21,
	SysDbgDisableKernelDebugger = 22,
	SysDbgGetAutoKdEnable = 23,
	SysDbgSetAutoKdEnable = 24,
	SysDbgGetPrintBufferSize = 25,
	SysDbgSetPrintBufferSize = 26,
	SysDbgGetKdUmExceptionEnable = 27,
	SysDbgSetKdUmExceptionEnable = 28,
	SysDbgGetTriageDump = 29,
	SysDbgGetKdBlockEnable = 30,
	SysDbgSetKdBlockEnable = 31,
	SysDbgRegisterForUmBreakInfo = 32,
	SysDbgGetUmBreakPid = 33,
	SysDbgClearUmBreakPid = 34,
	SysDbgGetUmAttachPid = 35,
	SysDbgClearUmAttachPid = 36,
} SYSDBG_COMMAND;

//
// KD Structures
//
typedef struct _KD_SYMBOLS_INFO
{
	PVOID BaseOfDll;
	ULONG_PTR ProcessId;
	ULONG CheckSum;
	ULONG SizeOfImage;
} KD_SYMBOLS_INFO, * PKD_SYMBOLS_INFO;

//
// Processor Execution Modes
//
typedef enum _MODE
{
	KernelMode,
	UserMode,
	MaximumMode
} MODE;

//
// Continue Status
//
typedef enum _KCONTINUE_STATUS
{
	ContinueError = 0,
	ContinueSuccess,
	ContinueProcessorReselected,
	ContinueNextProcessor
} KCONTINUE_STATUS;
