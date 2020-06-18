#pragma once
#include "bubble.hpp"

using namespace std;

BubbleSort::BubbleSort(const int width, const int height)
	: SortWindow(width, height)
{
	lastunsortedindex = values.size()-1;
}

void BubbleSort::Swap()
{
	swap(values[innercounter], values[innercounter+1]);
	swapped = true;
	innercounter++;
	changeState(SortingState::Compare);
}

void BubbleSort::Compare() 
{
	if(loopstate == LoopState::Outer)
	{
		swapped = false;
		innercounter = 0;
		loopstate = LoopState::Inner;
	}

	setCompared(innercounter, innercounter+1);
	if(innercounter < lastunsortedindex)
	{
		if(values[innercounter] > values[innercounter+1])
		{
			changeState(SortingState::Swap);
		}
		else innercounter++;
	}
	else
	{
		loopstate = LoopState::Outer;
		addSorted(lastunsortedindex--);
		if(!swapped)
		{
			changeState(SortingState::Done);
			for(int i = 0; i <= lastunsortedindex; i++)
				addSorted(i);
		}
	}
}