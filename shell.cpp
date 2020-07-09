#pragma once
#include "shell.hpp"

using namespace std;

ShellSort::ShellSort(const int width, const int height)
	: SortWindow(width, height)
{
	gaps.assign({701, 301, 132, 57, 23, 10, 4, 1});
	innercounter = 0;
	outercounter = -1;
	for(int gap : gaps)
		if(gap > values.size())
			outercounter++;
}

void ShellSort::Swap()
{
	swap(values[innerstcounter], values[innerstcounter-gaps[outercounter]]);
	innerstcounter -= gaps[outercounter];
	changeState(SortingState::Compare);
}

void ShellSort::Compare()
{
	if(loopstate == LoopState::Outer)
	{
		//TODO: change direct access to loopstate in all classes
		loopstate = LoopState::Inner;
		outercounter++;
		if(outercounter == gaps.size())
		{
			changeState(SortingState::Done);
			return;
		}
		innercounter = gaps[outercounter];
	}
	
	if(loopstate == LoopState::Inner)
	{
		loopstate = LoopState::Innerst;
		innerstcounter = innercounter;
	}
	
	if(innerstcounter >= gaps[outercounter]
		&& values[innerstcounter-gaps[outercounter]] > values[innerstcounter])
	{
		changeState(SortingState::Swap);
		setCompared(innerstcounter-gaps[outercounter], innerstcounter);
	}
	else
	{
		innercounter++;
		if(innercounter >= values.size())
			changeState(LoopState::Outer);
		else
			changeState(LoopState::Inner);
	}
}