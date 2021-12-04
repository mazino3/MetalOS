#include <MetalOS.UI.h>
#include <Debug.h>
#include <stdio.h>

Channel UIChannel = {};

void CreateUIChannel()
{
	DebugPrintf("CreateUIChannel\n");
	
	ProcessInfo info = { 0 };
	GetProcessInfo(info);
	DebugPrintf("Proccess Id %d\n", info.Id);
	
	//Open desktop channel
	void* outboundAddress = MapSharedObject(nullptr, "Desktop");
	Assert(outboundAddress);
	UIChannel.Outbound.Header = static_cast<RingBufferHeader*>(outboundAddress);
	UIChannel.Outbound.Buffer = reinterpret_cast<char*>((uintptr_t)outboundAddress + PAGE_SIZE);
	DebugPrintf("  Outbound - Header: 0x%016x, Buffer: 0x%016x\n", UIChannel.Outbound.Header, UIChannel.Outbound.Buffer);
	
	//Create inbound channel
	char name[64] = { 0 };
	sprintf(name, "Desktop_to_%d", info.Id);
	HRingBuffer ringBuffer = CreateRingBuffer(name, sizeof(RingBufferHeader), UIChannelSize);
	void* address = MapObject(nullptr, ringBuffer);

	UIChannel.Inbound.Header = static_cast<RingBufferHeader*>(address);
	UIChannel.Inbound.Buffer = reinterpret_cast<char*>((uintptr_t)address + PAGE_SIZE);
	DebugPrintf("  Inbound - Header: 0x%016x, Buffer: 0x%016x\n", UIChannel.Inbound.Header, UIChannel.Inbound.Buffer);
}

uint32_t __declspec(dllexport) DllMain(Handle handle, DllEntryReason reason)
{
	switch (reason)
	{
	case DllEntryReason::ProcessAttach:
	{
		CreateUIChannel();
	}
	break;

	default:
		//Do nothing
		break;
	}

	return true;
}
