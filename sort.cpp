#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;
using namespace olc;

Pixel RandomPixel(const int mod = 255)
{
	return{uint8_t(rand()%mod),uint8_t(rand()%mod),uint8_t(rand()%mod)};
}

class Subwindow
{
private:
	Sprite sprite;

protected:
	bool SetPixel(int x, int y, const Pixel& pixel)
	{
		return sprite.SetPixel(x, y, pixel);
	}

	Subwindow(const int width, const int height)
		: sprite(width, height)
	{}

public:
	virtual void Update() = 0;

	int GetWidth() { return sprite.width; }
	int GetHeight() { return sprite.height; }

	Sprite* GetSprite() { return &sprite; }
};

class DummyWindow : public Subwindow
{
private:
	Pixel color;

public:
	DummyWindow(const int width, const int height, const Pixel color)
		: color{color}, Subwindow(width, height)
	{}

	void Update() override
	{
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
		SetPixel(rand()%GetWidth(),rand()%GetHeight(),color);
	}
};

struct SubwindowStorage
{
	int xoffset;
	int yoffset;
	int scale;
	unique_ptr<Subwindow> subwindow;
};

class OLC_SORT : public olc::PixelGameEngine
{
private:
	vector<SubwindowStorage> subwindows;

public:
	OLC_SORT()
	{
		sAppName = "Sort Stuff";
	}

private:
	void DrawSubwindows()
	{
		for (auto& storage : subwindows)
		{
			storage.subwindow->Update();
			DrawSprite(	storage.xoffset,
						storage.yoffset,
						storage.subwindow->GetSprite(),
						storage.scale);
		}
	}

	void CreateSubwindows(const int rows, const int collumns, const int scale)
	{
		int rowoffset = ScreenHeight()/rows;
		int collumnoffset = ScreenWidth()/collumns;
		int width = (collumnoffset-3)/scale;
		int w = (fmodf((float)collumnoffset/(float)scale,1.f)*scale)/2;
		int height = (rowoffset-8-2)/scale;
		int h = (fmodf((float)rowoffset/(float)scale,1.f)*scale)/2;
		
		subwindows.reserve(rows*collumns);

		for (int yoffset = 8+h; yoffset < rowoffset*rows; yoffset+=rowoffset)
		{
			for (int xoffset = 0+w; xoffset < collumnoffset*collumns; xoffset+=collumnoffset)
			{
				Pixel pixel = RandomPixel();
				subwindows.push_back({
					xoffset+2,
					yoffset+1,
					scale,
					unique_ptr<Subwindow>(
						new DummyWindow{width,height, pixel})
					});

				DrawString(xoffset, yoffset-8," X:"+to_string(xoffset)+" Y:"+to_string(yoffset),WHITE);
				DrawRect(xoffset+1, yoffset, (width*scale)+1, (height*scale)+1, pixel);
			}
		}
	}

	void SetText(const int subwindowindex, const string& text)
	{
		if(subwindowindex < 0 || subwindowindex >= subwindows.size()) return;

		FillRect(subwindows[subwindowindex].xoffset-1,
				subwindows[subwindowindex].yoffset-9,
				(subwindows[subwindowindex].subwindow->GetWidth()*subwindows[subwindowindex].scale)+1,
				8, BLACK);

		DrawString(subwindows[subwindowindex].xoffset,
					subwindows[subwindowindex].yoffset-9,
					text, WHITE);
	}

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		CreateSubwindows(2, 3, 4);
		SetText(0, "First Thingy");
		SetText(5, "Sixth Thingy");

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		DrawSubwindows();

		return true;
	}
};


int main()
{
	OLC_SORT game;
	game.Construct(795, 503, 2, 2, false, true);
	game.Start();

	return 0;
}
