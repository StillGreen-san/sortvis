#pragma once
#include "selection.hpp"

using namespace std;

SelectionSort::SelectionSort(const int width, const int height)
	: SortWindow(width, height)
{
	firstunsortedindex = 0;
	outercounter = 0;
}

void SelectionSort::Swap()
{
	swap(values[minimumindex], values[firstunsortedindex]);
	addSorted(firstunsortedindex++);
	outercounter++;
	changeState(SortingState::Compare);
}

void SelectionSort::Compare()
{
	if(outercounter == values.size())
	{
		changeState(SortingState::Done);
		return;
	}
	
	if(loopstate == LoopState::Outer)
	{
		minimumindex = firstunsortedindex;
		loopstate = LoopState::Inner;
		innercounter = firstunsortedindex;
	}

	if(innercounter < values.size())
	{
		if(values[innercounter] < values[minimumindex])
			minimumindex = innercounter;
		setCompared(innercounter, minimumindex);
		innercounter++;
	}
	else
	{
		changeState(LoopState::Outer);
		changeState(SortingState::Swap);
	}
}