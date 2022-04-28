#pragma once

//#include "Device.h"
#include <string>
#include "MetalOS.Kernel.h"
#include "KFile.h"

class Device;
class Driver
{
public:
	Driver(Device& device);
	
	virtual Result Initialize() = 0;
	virtual Result Read(char* buffer, size_t length, size_t* bytesRead = nullptr) = 0;
	virtual Result Write(const char* buffer, size_t length) = 0;
	virtual Result EnumerateChildren() = 0;

protected:
	Device& m_device;
};

class HardDriveDriver
{
public:
	virtual KFile* OpenFile(const std::string& path, const GenericAccess access) const = 0;
	virtual size_t ReadFile(const KFile& handle, void* const buffer, const size_t bytesToRead) const = 0;
};

