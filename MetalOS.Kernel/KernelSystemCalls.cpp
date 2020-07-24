#include "Kernel.h"
#include "Main.h"
#include "UserWindow.h"

//TODO: kernel method to validate user pointer

SystemCallResult Kernel::GetSystemInfo(SystemInfo* info)
{
	if (!info)
		return SystemCallResult::Failed;
	
	info->PageSize = PAGE_SIZE;
	info->Architecture = SystemArchitecture::x64;
	return SystemCallResult::Success;
}

//Milliseconds
size_t Kernel::GetTickCount()
{
	const nano100_t tsc = m_hyperV->ReadTsc();
	return (tsc * 100) / 1000000;
}

void Kernel::Sleep(const uint32_t milliseconds)
{
	if (!milliseconds)
		return;

	KernelThreadSleep((nano_t)milliseconds * 1000 * 1000);
}

SystemCallResult Kernel::ExitProcess(const uint32_t exitCode)
{
	UserProcess& process = m_scheduler->GetCurrentProcess();
	process.Delete = true;
	Print("Process: %s exited with code 0x%x\n", process.GetName().c_str(), exitCode);
	m_scheduler->KillThread();
	return SystemCallResult::Success;
}

SystemCallResult Kernel::ExitThread(const uint32_t exitCode)
{
	UserProcess& process = m_scheduler->GetCurrentProcess();
	Print("Thread of %s exited with code 0x%x\n", process.GetName().c_str(), exitCode);
	m_scheduler->KillThread();
	return SystemCallResult::Success;
}

SystemCallResult Kernel::CreateWindow(const char* name)
{
	if (!name)
		return SystemCallResult::Failed;

	//One window at at time
	Assert(Window == nullptr);

	Rectangle rect = { 0 };
	rect.P1.X = 0;
	rect.P1.Y = 0;
	rect.P2.X = m_display->GetWidth();
	rect.P2.Y = m_display->GetHeight();

	UserThread* user = m_scheduler->GetCurrentUserThread();
	Assert(user);

	UserWindow* window = new UserWindow(name, rect, *user);
	user->Window = window;
	this->Window = window;

	return SystemCallResult::Success;
}

SystemCallResult Kernel::GetWindowRect(const Handle handle, Rectangle* rect)
{
	if (!rect)
		return SystemCallResult::Failed;

	UserThread* user = m_scheduler->GetCurrentUserThread();
	if (!user->Window)
		return SystemCallResult::Failed;

	//Copy rectangle
	*rect = user->Window->GetRectangle();

	return SystemCallResult::Success;
}

//Blocks until message and returns it
SystemCallResult Kernel::GetMessage(Message* message)
{
	if (!message)
		return SystemCallResult::Failed;

	UserThread* user = m_scheduler->GetCurrentUserThread();
	if (!user->Window)
		return SystemCallResult::Failed;

	Message* msg = m_scheduler->MessageWait(); //Can block
	*message = *msg;
	return SystemCallResult::Success;
}

//Doesn't block
SystemCallResult Kernel::PeekMessage(Message* message)
{
	UserThread* user = m_scheduler->GetCurrentUserThread();
	Assert(user);

	Message* msg = user->DequeueMessage();
	if (!msg)
		return SystemCallResult::Failed;
	
	*message = *msg;
	return SystemCallResult::Success;
}

SystemCallResult Kernel::SetScreenBuffer(void* buffer)
{
	if (!buffer)
		return SystemCallResult::Failed;

	//Display time
	/*
	EFI_TIME time = { 0 };
	m_runtime.GetTime(&time, nullptr);
	Print("  Date: %02d-%02d-%02d %02d:%02d:%02d\r\n", time.Month, time.Day, time.Year, time.Hour, time.Minute, time.Second);
	*/

	const size_t size = (size_t)m_display->GetHeight() * m_display->GetWidth();
	//Printf("D: 0x%016x, S: 0x%016x\n", (void*)m_display->Buffer(), buffer);
	memcpy((void*)m_display->Buffer(), buffer, sizeof(Color) * size);

	return SystemCallResult::Success;
}

Handle Kernel::CreateFile(const char* name, const GenericAccess access)
{
	if (!name)
		return nullptr;

	Print("CreateFile: %s Access: %d\n", name, access);

	return this->CreateFile(std::string(name), access);
}

SystemCallResult Kernel::ReadFile(const Handle handle, void* buffer, const size_t bufferSize, size_t* bytesRead)
{
	if (!handle || !buffer || !bufferSize)
		return SystemCallResult::Failed;

	FileHandle* file = (FileHandle*)handle;

	bool result = this->ReadFile(file, buffer, bufferSize, bytesRead);
	return result ? SystemCallResult::Success : SystemCallResult::Failed;
}

SystemCallResult Kernel::WriteFile(const Handle handle, const void* lpBuffer, size_t bufferSize, size_t* bytesWritten)
{
	//Not implemented
	Assert(false);
	SystemCallResult::Failed;
}

SystemCallResult Kernel::SetFilePointer(const Handle handle, const __int64 position, const FilePointerMove moveType, size_t* newPosition)
{
	if (!handle)
		return SystemCallResult::Failed;

	FileHandle* file = (FileHandle*)handle;
	
	size_t pos;
	switch (moveType)
	{
	case FilePointerMove::Begin:
		pos = position;
		break;

	case FilePointerMove::Current:
		pos = file->Position + position;
		break;

	case FilePointerMove::End:
		pos = file->Length + position;
		break;

	default:
		return SystemCallResult::Failed;
	}

	bool result = this->SetFilePosition(file, pos);
	if (result)
	{
		if (newPosition != nullptr)
			*newPosition = pos;
		return SystemCallResult::Success;
	}
	
	return SystemCallResult::Failed;
}

SystemCallResult Kernel::CloseFile(const Handle handle)
{
	if (!handle)
		return SystemCallResult::Failed;

	CloseFile((FileHandle*)handle);
	return SystemCallResult::Success;
}

SystemCallResult Kernel::MoveFile(const char* existingFileName, const char* newFileName)
{
	Assert(false);
	SystemCallResult::Failed;
}

SystemCallResult Kernel::DeleteFile(const char* fileName)
{
	Assert(false);
	SystemCallResult::Failed;
}

SystemCallResult Kernel::CreateDirectory(const char* path)
{
	Assert(false);
	SystemCallResult::Failed;
}

void* Kernel::VirtualAlloc(const void* address, const size_t size, const MemoryAllocationType allocationType, const MemoryProtection protect)
{
	if (!size)
		return nullptr;

	UserProcess& process = m_scheduler->GetCurrentProcess();
	
	return m_virtualMemory->Allocate((uintptr_t)address, SIZE_TO_PAGES(size), protect, process.GetAddressSpace());
}

SystemCallResult Kernel::DebugPrint(char* s)
{
	this->m_printer->Write(s);
	return SystemCallResult::Success;
}
