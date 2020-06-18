#pragma once
#include "olcPixelGameEngine.h"

class Subwindow
{
private:
	olc::Sprite sprite;

protected:
	bool SetPixel(int x, int y, const olc::Pixel& pixel);

	bool SetLine(int x1, int y1, int x2, int y2, const olc::Pixel& pixel);

	Subwindow(const int width, const int height);

public:
	virtual void Update() = 0;

	int GetWidth();
	int GetHeight();

	olc::Sprite* GetSprite();
};

class DummyWindow : public Subwindow
{
private:
	olc::Pixel color;

public:
	DummyWindow(const int width, const int height, const olc::Pixel color);

	void Update() override;
};