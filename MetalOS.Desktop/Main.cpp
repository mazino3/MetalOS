#include <MetalOS.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Debug.h>
#include <new>
#include "Display.h"

void ProcessMessage(const Message& message)
{
	switch (message.Header.MessageType)
	{
	case MessageType::KeyEvent:
		if (message.KeyEvent.Key >= 'A' && message.KeyEvent.Key <= 'Z')
			DebugPrintf("Key: %c - %d\n", message.KeyEvent.Key, message.KeyEvent.Flags.Pressed);
		else
			DebugPrintf("Key: 0x%x - %d\n", message.KeyEvent.Key, message.KeyEvent.Flags.Pressed);
		break;

	default:
		DebugPrintf("Event: 0x%x\n", message.Header.MessageType);
		break;
	}
}

int main(int argc, char** argv)
{
	SystemInfo sysInfo = { 0 };
	GetSystemInfo(sysInfo);
	DebugPrintf("PageSize: 0x%x\n", sysInfo.PageSize);

	ProcessInfo info = { 0 };
	GetProcessInfo(info);
	DebugPrintf("ProcessID: 0x%x\n", info.Id);

	Handle desktop;
	Point2D bounds;
	CreateDesktop(desktop, bounds);
	DebugPrintf("Bounds: (0x%x,0x%x)\n", bounds.X, bounds.Y);

	Thread* thread = Thread::GetCurrent();
	DebugPrintf("ThreadID: 0x%x\n", thread->GetId());

	Display display(bounds.Y, bounds.X);
	display.Initialize();
	DebugPrintf("Allocated Buffer: 0x%016x\n", display.Buffer());

	//Create Channel for incoming messages
	HRingBuffer ringBuffer = CreateRingBuffer("Desktop", sizeof(RingBufferHeader), UIChannelSize);
	DebugPrintf("RingBuffer: 0x%016x\n", ringBuffer);
	void* address = MapObject(nullptr, ringBuffer);
	DebugPrintf("Mapped: 0x%016x\n", address);

	RingBufferHeader*  header = static_cast<RingBufferHeader*>(address);
	char* buffer = reinterpret_cast<char*>((uintptr_t)address + PAGE_SIZE);
	DebugPrintf("CreateWindow - Header: 0x%016x, Buffer: 0x%016x\n");

	SystemCallResult r = CreateProcess("calc.exe");
	DebugPrintf("CreateProcess %d\n", r);

	Message message = {};
	while (true)
	{
		//Process all input
		while (PeekMessage(message) == SystemCallResult::Success)
		{
			ProcessMessage(message);
		}

		//Write output
		SetScreenBuffer(display.Buffer());

		//Sleep
		Sleep(1000 / 30);
	}
}
