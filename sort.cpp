#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;
using namespace olc;

class SubWindow
{
private:
	vector<Pixel> mBuffer;
	int mWidth, mHeight;

protected:
	void SetPixel(int aX, int aY, const Pixel& aP)
	{
		if(aX >= mWidth || aY >= mHeight) return;
		mBuffer[aY*mWidth+aX] = aP;
	}

	SubWindow(const int aWidth, const int aHeight)
		: mWidth(aWidth), mHeight(aHeight), mBuffer(aWidth*aHeight, BLACK)
	{}

public:
	virtual void Update() = 0;

	int WindowWidth() { return mWidth; }
	int WindowHeight() { return mHeight; }

	const vector<Pixel>* GetBuffer()
	{
		return &mBuffer;
	}
};

class DummyWindow : SubWindow
{
public:
	DummyWindow(const int aWidth, const int aHeight)
		: SubWindow(aWidth, aHeight)
	{
		for (int llX = 0; llX < aWidth; llX++)
			SetPixel(llX, 0, WHITE);
		for (int llY = 0; llY < aWidth; llY++)
			SetPixel(llY, 0, WHITE);
	}

	void Update() override
	{
		SetPixel(rand()%WindowWidth(),rand()%WindowHeight(),BLACK);
		SetPixel(rand()%WindowWidth(),rand()%WindowHeight(),BLACK);
		SetPixel(rand()%WindowWidth(),rand()%WindowHeight(),BLACK);
		SetPixel(rand()%WindowWidth(),rand()%WindowHeight(),YELLOW);
	}
};

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
