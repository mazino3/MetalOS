#include "LoadingScreen.h"
#include "Colors.h"
#include "Main.h"
#include <algorithm>
//#include <crt_stdlib.h>

//BGR pixels
const Color LoadingScreen::FireColors[] =
{
	{ 0x07, 0x07, 0x07, 0x00},
	{ 0x07, 0x07, 0x1F, 0x00},
	{ 0x07, 0x0F, 0x2F, 0x00},
	{ 0x07, 0x0F, 0x47, 0x00},
	{ 0x07, 0x17, 0x57, 0x00},
	{ 0x07, 0x1F, 0x67, 0x00},
	{ 0x07, 0x1F, 0x77, 0x00},
	{ 0x07, 0x27, 0x8F, 0x00},
	{ 0x07, 0x2F, 0x9F, 0x00},
	{ 0x07, 0x3F, 0xAF, 0x00},
	{ 0x07, 0x47, 0xBF, 0x00},
	{ 0x07, 0x47, 0xC7, 0x00},
	{ 0x07, 0x4F, 0xDF, 0x00},
	{ 0x07, 0x57, 0xDF, 0x00},
	{ 0x07, 0x57, 0xDF, 0x00},
	{ 0x07, 0x5F, 0xD7, 0x00},
	{ 0x07, 0x5F, 0xD7, 0x00},
	{ 0x0F, 0x67, 0xD7, 0x00},
	{ 0x0F, 0x6F, 0xCF, 0x00},
	{ 0x0F, 0x77, 0xCF, 0x00},
	{ 0x0F, 0x7F, 0xCF, 0x00},
	{ 0x17, 0x87, 0xCF, 0x00},
	{ 0x17, 0x87, 0xC7, 0x00},
	{ 0x17, 0x8F, 0xC7, 0x00},
	{ 0x1F, 0x97, 0xC7, 0x00},
	{ 0x1F, 0x9F, 0xBF, 0x00},
	{ 0x1F, 0x9F, 0xBF, 0x00},
	{ 0x27, 0xA7, 0xBF, 0x00},
	{ 0x27, 0xA7, 0xBF, 0x00},
	{ 0x2F, 0xAF, 0xBF, 0x00},
	{ 0x2F, 0xAF, 0xB7, 0x00},
	{ 0x2F, 0xB7, 0xB7, 0x00},
	{ 0x37, 0xB7, 0xB7, 0x00},
	{ 0x6F, 0xCF, 0xCF, 0x00},
	{ 0x9F, 0xDF, 0xDF, 0x00},
	{ 0xC7, 0xEF, 0xEF, 0x00},
	{ 0xFF, 0xFF, 0xFF, 0x00},
};

LoadingScreen::LoadingScreen(Display& display) :
	m_display(display),
	m_indexes(display.GetHeight() / PixelSize, display.GetWidth() / PixelSize),
	m_buffer(display.GetHeight(), display.GetWidth())
{
	//Justify above integer divisions
	Assert(display.GetHeight() % PixelSize == 0);
	Assert(display.GetWidth() % PixelSize == 0);

	static_assert(sizeof(LoadingScreen::FireColors) == FireColorsCount * sizeof(Color));
}

void LoadingScreen::Initialize()
{
	Print("LoadingScreen::Initialize\n");

	//Initialize indexes to 0
	for (size_t y = 0; y < m_indexes.GetHeight(); y++)
		for (size_t x = 0; x < m_indexes.GetWidth(); x++)
			m_indexes.Set({ x, y }, 0);

	//Set bottom of indexes to highest index
	const size_t bottomY = m_indexes.GetHeight() - 1;
	for (size_t x = 0; x < m_indexes.GetWidth(); x++)
		m_indexes.Set({ x, bottomY }, FireColorsCount - 1);

	Draw();

	//Create thread
	kernel.CreateThread(LoadingScreen::ThreadLoop, this);
}

void LoadingScreen::Update()
{
	Print("LoadingScreen::Update\n");
	DoFire();
}

void LoadingScreen::Draw()
{
	Print("LoadingScreen::Draw\n");
	
	//Clear Screen
	m_display.ColorScreen(Colors::Black);

	//Populate graphics buffer based on indexes
	for (size_t x = 0; x < m_display.GetWidth(); x++)
		for (size_t y = 0; y < m_display.GetHeight(); y++)
		{
			//Get index based on scaled pixel size
			const Point2D scaled = { x / PixelSize, y / PixelSize };
			const uint8_t index = m_indexes.Get(scaled);
			const Color c = FireColors[index];

			m_display.SetPixel(c, { x, y });
		}
}

uint32_t LoadingScreen::ThreadLoop(void* arg)
{
	LoadingScreen* screen = (LoadingScreen*)arg;
	while (true)
	{
		screen->Update();
		screen->Draw();

		kernel.Sleep(SECOND / 16);
	}
}

void LoadingScreen::DoFire()
{
	for (size_t x = 0; x < m_indexes.GetWidth(); x++)
		for (size_t y = 1; y < m_indexes.GetHeight(); y++)
			SpreadFire({ x, y });
}

void LoadingScreen::SpreadFire(Point2D point)
{
	//Print("SpreadFire: %d x %d\n", point.X, point.Y);
	uint8_t index = m_indexes.Get(point);
	//Print("  1\n");
	if (index == 0)
	{
		//Print("  2\n");
		m_indexes.Set({ point.X, point.Y-- }, 0);
		//Print("  3\n");
	}
	else
	{
		const size_t src = point.Y * m_indexes.GetWidth() + point.X;
		const int randIndex = rand() & 3;
		const size_t dst = src - randIndex + 1;
		//const Point2D dstPoint = { dst % m_indexes.GetWidth() - 1, dst / m_indexes.GetWidth() };
		const Point2D dstPoint = { point.X, point.Y - 1 };
		//Print("  Dest: %d x %d\n", dstPoint.X, dstPoint.Y);
		m_indexes.Set(dstPoint, index - (randIndex & 1));
		//Print("  5\n");
	}
}


