#include <MetalOS.UI.h>

void Callback(HWindow handle, Message& message)
{

}

int main(int argc, char** argv)
{
	DebugPrint("Hi From calc!\n");
	
	WindowStyle style = {};
	style.IsBordered = true;
	style.Callback = Callback;

	Handle h = CreateWindow("Calc", 50, 50, 100, 100, style);

	Message message = {};
	while (GetMessage(message) == SystemCallResult::Success)
	{
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
	}
}
