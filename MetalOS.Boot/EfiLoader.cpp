#include "EfiLoader.h"
#include "BootLoader.h"
#include "WindowsPE.h"
#include "Memory.h"
#include "Error.h"
#include <LoaderParams.h>
#include <Kernel.h>
#include <crt_string.h>

//#include <Windows.h>

EFI_GUID gEfiLoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID gEfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

typedef void (*CrtInitializer)();

//This method should check the memory map file and ensure nobody else has this reservation
EFI_STATUS EfiLoader::MapKernel(EFI_FILE* file, UINT64* pImageSizeOut, UINT64* pEntryPointOut, EFI_PHYSICAL_ADDRESS* pPhysicalImageBase)
{
	EFI_STATUS status;

	//Dos header
	UINTN size = sizeof(IMAGE_DOS_HEADER);
	IMAGE_DOS_HEADER dosHeader;
	ReturnIfNotSuccess(file->Read(file, &size, &dosHeader));
	ReturnIfNotSuccess(dosHeader.e_magic);

	//NT Header
	size = sizeof(IMAGE_NT_HEADERS64);
	IMAGE_NT_HEADERS64 peHeader;
	ReturnIfNotSuccess(file->SetPosition(file, (UINT64)dosHeader.e_lfanew));
	ReturnIfNotSuccess(file->Read(file, &size, &peHeader));

	//Verify image
	if (peHeader.Signature != IMAGE_NT_SIGNATURE ||
		peHeader.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
		peHeader.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
		peHeader.OptionalHeader.Subsystem != IMAGE_SUBSYSTEM_NATIVE)
		return EFI_UNSUPPORTED;

	//Kernel cant have imports yet
	if (peHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size != 0)
		ReturnIfNotSuccess(EFI_UNSUPPORTED);

	//Allocate pages for full image
	ReturnIfNotSuccess(BS->AllocatePages(AllocateAnyPages, EfiLoaderData, EFI_SIZE_TO_PAGES((UINTN)peHeader.OptionalHeader.SizeOfImage), pPhysicalImageBase));

	//Read headers into memory
	size = peHeader.OptionalHeader.SizeOfHeaders;
	ReturnIfNotSuccess(file->SetPosition(file, 0));
	ReturnIfNotSuccess(file->Read(file, &size, (void*)*pPhysicalImageBase));

	//Pointer into NTHeader loaded in memory
	PIMAGE_NT_HEADERS64 pNtHeader = (PIMAGE_NT_HEADERS64)(*pPhysicalImageBase + dosHeader.e_lfanew);
	
	//Write sections into memory
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION_64(pNtHeader);
	for (WORD i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	{
		EFI_PHYSICAL_ADDRESS destination = *pPhysicalImageBase + section[i].VirtualAddress;

		//If physical size is non-zero, read data to allocated address
		UINTN rawSize = section[i].SizeOfRawData;
		if (rawSize != 0)
		{
			ReturnIfNotSuccess(file->SetPosition(file, section[i].PointerToRawData));
			ReturnIfNotSuccess(file->Read(file, &rawSize, (void*)destination));
		}
	}

	//Update NTHeader to point to new virtual address
	pNtHeader->OptionalHeader.ImageBase = KernelBaseAddress;

	//Relocate image to KernelSpace. It gets allocated at KernelStart + ImageBase
	IMAGE_DATA_DIRECTORY relocationDirectory = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	Print(L"  Relocations: %u\r\n", relocationDirectory.Size);
	if (relocationDirectory.Size)
	{
		PIMAGE_BASE_RELOCATION pBaseRelocation = (PIMAGE_BASE_RELOCATION)(*pPhysicalImageBase + relocationDirectory.VirtualAddress);

		//Calculate relative shift by subtracing location live ImageBase from ImageBase first read from image
		UINT64 delta = (UINT64)pNtHeader->OptionalHeader.ImageBase - (UINT64)peHeader.OptionalHeader.ImageBase;

		while (pBaseRelocation->VirtualAddress)
		{
			PBYTE locationBase = (PBYTE)(*pPhysicalImageBase + pBaseRelocation->VirtualAddress);
			PWORD locationData = (PWORD)((UINT64)pBaseRelocation + sizeof(IMAGE_BASE_RELOCATION));

			for (DWORD i = 0; i < (pBaseRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD); i++, locationData++)
			{
				int type = (*locationData >> 12);
				int offset = (*locationData & 0x0FFF);

				switch (type)
				{
				case IMAGE_REL_BASED_ABSOLUTE:
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					*(DWORD*)(locationBase + offset) += delta;
					break;

				case IMAGE_REL_BASED_DIR64:
					*(UINT64*)(locationBase + offset) += delta;
					break;
				}
			}

			pBaseRelocation = (PIMAGE_BASE_RELOCATION)((UINT64)pBaseRelocation + pBaseRelocation->SizeOfBlock);
		}
	}

	//Populate return variables
	*pImageSizeOut = pNtHeader->OptionalHeader.SizeOfImage;
	*pEntryPointOut = pNtHeader->OptionalHeader.ImageBase + pNtHeader->OptionalHeader.AddressOfEntryPoint;

	Print(L"  ImageBase: 0x%016x ImageSize: 0x%08x\r\n", KernelBaseAddress, EFI_SIZE_TO_PAGES(*pImageSizeOut));
	Print(L"  Entry: 0x%016x Physical: 0x%016x\r\n", *pEntryPointOut, *pPhysicalImageBase);

	return EFI_SUCCESS;
}

//This function doesn't do any error checking, should it?
EFI_STATUS EfiLoader::CrtInitialization(UINT64 imageBase)
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageBase;
	PIMAGE_NT_HEADERS64 pNtHeader = (PIMAGE_NT_HEADERS64)(imageBase + dosHeader->e_lfanew);

	//Find CRT section
	PIMAGE_SECTION_HEADER crtSection = nullptr;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION_64(pNtHeader);
	for (WORD i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	{
		if (strcmp((char*)& section[i].Name, ".CRT") == 0)
		{
			crtSection = &section[i];
			break;
		}
	}
	if (crtSection == nullptr)
		return EFI_NOT_FOUND;
	
	//https://docs.microsoft.com/en-us/cpp/c-runtime-library/crt-initialization?view=vs-2019
	//https://docs.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-warning-lnk4210?view=vs-2019
	//.CRT seems to be a list of function pointers (see asm). Loop through each one and invoke them
	CrtInitializer* initializer = (CrtInitializer*)(imageBase + crtSection->VirtualAddress);
	while (*initializer)
	{
		(*initializer)();
		initializer++;
	}

	return EFI_SUCCESS;
}
