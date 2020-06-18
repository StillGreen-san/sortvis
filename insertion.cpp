#pragma once
#include "insertion.hpp"

using namespace std;

InsertionSort::InsertionSort(const int width, const int height)
	: SortWindow(width, height)
{
	outercounter = 0;
	lastsortedindex = 0;
}

//TODO: check if actually ionsertion sort (insertion overhead)
void InsertionSort::Swap()
{
	if(loopstate == LoopState::Inner)
	{
		swap(values[innercounter], values[elementxindex]);
		elementxindex = innercounter--;
	}
	changeState(SortingState::Compare);
	if(innercounter == -1)
		changeState(LoopState::Outer);
}

void InsertionSort::Compare()
{
	if(loopstate == LoopState::Outer)
	{
		addSorted(outercounter);
		lastsortedindex = outercounter++;
		
		if(!(outercounter < values.size()))
		{
			changeState(SortingState::Done);
			return;
		}
		elementxindex = outercounter;
		innercounter = lastsortedindex;
		changeState(LoopState::Inner);
	}

	if(innercounter >= 0)
	{
		setCompared(innercounter, elementxindex);
		if(values[innercounter] > values[elementxindex])
		{
			changeState(SortingState::Swap);
		}
		else
		{
			changeState(SortingState::Swap);
			changeState(LoopState::Outer);
		}
	}
}
