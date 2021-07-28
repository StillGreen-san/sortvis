#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class SortVis : public olc::PixelGameEngine
{
public:
	SortVis()
	{
		sAppName = "Sorting Visualization | powered by olcPixelGameEngine";
	}

protected:
	// Called by olcConsoleGameEngine
	bool OnUserCreate() override
	{
		return true;
	}

	// Called by olcConsoleGameEngine
	bool OnUserUpdate(float /*fElapsedTime*/) override
	{
		return true;
	}
};

int main()
{
	SortVis vis;

	constexpr int32_t windowWidth = 400;
	constexpr int32_t windowHeight = 350;
	constexpr int32_t pixelSize = 2;

	olc::rcode rcode = vis.Construct(windowWidth, windowHeight, pixelSize, pixelSize, false, true, false);
	if(rcode != olc::OK)
	{
		return 1;
	}

	rcode = vis.Start();
	if(rcode != olc::OK)
	{
		return 1;
	}

	return 0;
}
