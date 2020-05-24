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

class DummyWindow : public SubWindow
{
public:
	DummyWindow(const int aWidth, const int aHeight)
		: SubWindow(aWidth, aHeight)
	{
		for (int llX = 0; llX < aWidth; llX++)
		{
			SetPixel(llX, 0, WHITE);
			SetPixel(llX, aHeight-1, WHITE);
		}
		for (int llY = 0; llY < aWidth; llY++)
		{
			SetPixel(0, llY, WHITE);
			SetPixel(aWidth-1, llY, WHITE);
		}
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
	vector<unique_ptr<SubWindow>> subwindows;

public:
	OLC_SORT()
	{
		sAppName = "Sort Stuff";
	}

private:
	void DrawSubWindow(const int aIndex, const int aXOffset, const int aYOffset)
	{
		if(aIndex < 0 || aIndex >= subwindows.size()) return;

		subwindows[aIndex]->Update();
		auto lBuffer = subwindows[aIndex]->GetBuffer();
		
		auto lCurrent = lBuffer->begin();
		auto lEnd = lBuffer->end();
		int lX = 0;
		int lY = 0;
		int lWidth = subwindows[aIndex]->WindowWidth();

		while (lCurrent != lEnd)
		{
			Draw(lX+aXOffset, lY+aXOffset, *lCurrent);

			lCurrent++;
			lX = (lX+1) % lWidth;
			if(lX == 0) lY++;
		}
	}

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		subwindows.push_back(
			unique_ptr<SubWindow>(new DummyWindow{30,30}));

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		DrawSubWindow(0, 250, 165);

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
