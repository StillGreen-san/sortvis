#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;
using namespace olc;

class OLC_SORT : public olc::PixelGameEngine
{
private:
	

public:
	OLC_SORT()
	{
		sAppName = "Sort Stuff";
	}

private:
	

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		Draw(rand()%ScreenWidth(), rand()%ScreenHeight());

		return true;
	}
};


int main()
{
	OLC_SORT game;
	game.Construct(1600, 1000, 1, 1, false, true);
	game.Start();

	return 0;
}
