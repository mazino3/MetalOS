#include <MetalOS.h>
#include <stdlib.h>
#include <new>
#include "Matrix.h"
#include "FireScreen.h"

int main(int argc, char** argv)
{
	Handle desktop;
	Point2D bounds;
	CreateDesktop(desktop, bounds);
	DebugPrintf("Bounds: (0x%x,0x%x)\n", bounds.X, bounds.Y);
	
	FireScreen screen(bounds.Y, bounds.X);
	screen.Initialize();
	screen.Draw();
	SetScreenBuffer(screen.GetBuffer());

	while (true)
	{
		screen.Update();
		screen.Draw();
		SetScreenBuffer(screen.GetBuffer());
	}
}


