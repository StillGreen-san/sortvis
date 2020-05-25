#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;
using namespace olc;

class Subwindow
{
private:
	Sprite sprite;
	int scale;

protected:
	bool SetPixel(int x, int y, const Pixel& pixel)
	{
		return sprite.SetPixel(x, y, pixel);
	}

	Subwindow(const int width, const int height, const int scale)
		: sprite(width, height), scale(scale)
	{}

public:
	virtual void Update() = 0;

	int GetWidth() { return sprite.width; }
	int GetHeight() { return sprite.height; }
	int GetScale() { return scale; }

	Sprite* GetSprite() { return &sprite; }
};

class DummyWindow : public Subwindow
{
private:
	Pixel color;

public:
	DummyWindow(const int width, const int height, const int scale, const Pixel color)
		: color{color}, Subwindow(width, height, scale)
	{}

	void Update() override
	{
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),color);
	}
};

class OLC_SORT : public olc::PixelGameEngine
{
private:
	vector<unique_ptr<Subwindow>> subwindows;

public:
	OLC_SORT()
	{
		sAppName = "Sort Stuff";
	}

private:
	void DrawSubWindow(const int index, const int xoffset, const int yoffset)
	{
		if(index < 0 || index >= subwindows.size()) return;

		auto& subwin = subwindows[index];
		subwin->Update();
		
		DrawSprite(xoffset, yoffset, subwin->GetSprite(), subwin->GetScale());
	}

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		subwindows.push_back(
			unique_ptr<Subwindow>(new DummyWindow{30,30,3,YELLOW}));

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
