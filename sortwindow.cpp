#include "sortwindow.hpp"
#include <algorithm>
#include <random>
#include <numeric>

using namespace std;
using namespace olc;

int MapInt(const int mina, const int maxa,const int minb, const int maxb, const int vala)
{
	int a = (vala - mina)*(maxb - minb);
	int b = maxa - mina;
	return minb + (a / b);
}

void SortWindow::changeState(SortingState state) 
{
	sortstate = state;
}

void SortWindow::changeState(LoopState state) 
{
	loopstate = state;
}

void SortWindow::addSorted(const int index) 
{
	sortedIndices[index] = true;
}

const vector<bool>& SortWindow::getSortedIndices() 
{
	return sortedIndices;
}

void SortWindow::setCompared(const int indexl, const int indexr) 
{
	comparedIndices[0] = indexl;
	comparedIndices[1] = indexr;
}

SortWindow::SortWindow(const int width, const int height) 
	: Subwindow(width, height), values(width), sortedIndices(width),
	sortstate(SortingState::Compare), comparedIndices(2), loopstate(LoopState::Outer),
	unsortedColor(BLUE), sortedColor(GREEN), comparedColor(YELLOW)
{
	iota(begin(values), end(values), 1);
	shuffle(begin(values), end(values), mt19937{random_device{}()});
	fill(begin(sortedIndices), end(sortedIndices), false);
	fill(begin(comparedIndices), end(comparedIndices), 0);
}

void SortWindow::Update() 
{
	if(++counter < 6) return;
	counter = 0;

	switch (sortstate)
	{
	case SortingState::Compare:
		Compare();
		break;
	case SortingState::Swap:
		Swap();
		break;
	}

	int canvaswidth = GetWidth()-1;
	int canvasheight = GetHeight()-1;		
	for(int x = 0; x < values.size(); x++)
	{
		int lineheight = MapInt(0,canvaswidth,0,canvasheight,values[x]);
		SetLine(x, 0, x, canvasheight-lineheight-1, BLACK);
		if(sortedIndices[x])
			SetLine(x, canvasheight-lineheight, x, canvasheight, sortedColor);
		else
			SetLine(x, canvasheight-lineheight, x, canvasheight, unsortedColor);
	}
	if(sortstate == SortingState::Done) return;
	for(int index : comparedIndices)
	{
		int lineheight = MapInt(0,canvaswidth,0,canvasheight,values[index]);
		SetLine(index, canvasheight-lineheight, index, canvasheight, comparedColor);
	}
}
