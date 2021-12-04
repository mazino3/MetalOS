#pragma once

#include <MetalOS.h>

#if !defined(EXPORT_UI)
#define UICALL(type) extern "C" __declspec(dllimport) type
#else
#define UICALL(type) extern "C" __declspec(dllexport) type
#endif

typedef void* HWindow;

typedef void (*WindowCallback)(HWindow handle, Message& message);

struct WindowStyle
{
	WindowCallback Callback;
	bool IsBordered;
};

static const char* DesktopChannel = "Desktop";

UICALL(HWindow) CreateWindow(const char* name, size_t x, size_t y, size_t width, size_t height, WindowStyle& style);
