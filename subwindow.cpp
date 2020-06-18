#include "subwindow.hpp"

using namespace std;
using namespace olc;

bool Subwindow::SetPixel(int x, int y, const Pixel& pixel) 
{
	return sprite.SetPixel(x, y, pixel);
}

bool Subwindow::SetLine(int x1, int y1, int x2, int y2, const Pixel& pixel) 
{
	bool hasdrawn = false;
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;

		if (dx == 0) // Line is vertical
		{
			if (y2 < y1) swap(y1, y2);
			for (y = y1; y <= y2; y++) hasdrawn |= SetPixel(x1, y, pixel);
			return hasdrawn;
		}

		if (dy == 0) // Line is horizontal
		{
			if (x2 < x1) swap(x1, x2);
			for (x = x1; x <= x2; x++) hasdrawn |= SetPixel(x, y1, pixel);
			return hasdrawn;
		}

		// Line is Funk-aye
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{ x = x1; y = y1; xe = x2; }
			else
			{ x = x2; y = y2; xe = x1; }

			hasdrawn |= SetPixel(x, y, pixel);

			for (i = 0; x<xe; i++)
			{
				x = x + 1;
				if (px<0)
					px = px + 2 * dy1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				hasdrawn |= SetPixel(x, y, pixel);
			}
		}
		else
		{
			if (dy >= 0)
			{ x = x1; y = y1; ye = y2; }
			else
			{ x = x2; y = y2; ye = y1; }

			hasdrawn |= SetPixel(x, y, pixel);

			for (i = 0; y<ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				hasdrawn |= SetPixel(x, y, pixel);
			}
		}
		return hasdrawn;
}

Subwindow::Subwindow(const int width, const int height)
	: sprite(width, height)
{
	
}

int Subwindow::GetWidth() 
{
	return sprite.width;
}

int Subwindow::GetHeight() 
{
	return sprite.height;
}

Sprite* Subwindow::GetSprite() 
{
	return &sprite;
}

DummyWindow::DummyWindow(const int width, const int height, const Pixel color)
	: color{color}, Subwindow(width, height)
{
	
}

void DummyWindow::Update() 
{
	SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
	SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
	SetPixel(rand()%GetWidth(),rand()%GetHeight(),BLACK);
	SetPixel(rand()%GetWidth(),rand()%GetHeight(),color);
}
