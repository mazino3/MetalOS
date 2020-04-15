#pragma once
#include "Display.h"
#include <array>

class LoadingScreen
{
public:
	LoadingScreen(Display& display);
	void Initialize();

private:
	static uint32_t ThreadLoop(void* arg);
	//DOOM Fire implmentation
	//http://fabiensanglard.net/doom_fire_psx/
	//JS implementation uses a index 'firePixels' which is as big as the whole screen
	//To save room, stuff the index into the reserved field in the color
	void Draw();
	void DoFire();
	void SpreadFire(Point2D point);

	static const Color FireColors[];

	Display& m_display;
};

