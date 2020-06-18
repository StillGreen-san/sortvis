#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "sortings.h"

using namespace std;
using namespace olc;

Pixel RandomPixel(const int mod = 255)
{
	return{uint8_t(rand()%mod),uint8_t(rand()%mod),uint8_t(rand()%mod)};
}

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

	template <class CLASS>
	void SetSubWindow(const int subwindowindex, const string& text)
	{
		if(subwindowindex < 0 || subwindowindex >= subwindows.size()) return;

		SetText(subwindowindex, text);
		swap(subwindows[subwindowindex].subwindow, unique_ptr<Subwindow>(
			new CLASS{subwindows[subwindowindex].subwindow->GetWidth(),
					subwindows[subwindowindex].subwindow->GetHeight()}));
	}

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		CreateSubwindows(2, 3, 4);
		SetSubWindow<BubbleSort>(4, "Bubble Sort");
		SetSubWindow<SelectionSort>(3, "Selection Sort");
		SetSubWindow<InsertionSort>(1, "Insertion Sort");
		SetSubWindow<QuickSort>(2, "Quick Sort");

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
