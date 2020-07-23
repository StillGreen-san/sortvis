#pragma once
#include "heap.hpp"

using namespace std;

HeapSort::HeapSort(const int width, const int height)
	: SortWindow(width, height)
{
	innercounter = values.size()-1;
	outercounter = iParent(innercounter);
}

void HeapSort::Swap()
{
	switch (loopstate)
	{
	case LoopState::Inner :
		swap(values[innercounter], values[0]);
		addSorted(innercounter);
		innercounter--;
		changeState(SortingState::Compare);
		changeState(LoopState::Innerst);
		previousloopstate = LoopState::Inner;
		break;
	case LoopState::Innerst :
		swap(values[innerstroot], values[innerstswap]);
		innerstroot = innerstswap;
		break;
	}
}

void HeapSort::Compare()
{
	if(loopstate == LoopState::Outer)
	{
		if(outercounter >= 0)
		{
			changeState(LoopState::Innerst);
			innerststep = 0;
			innerstroot = outercounter;
			previousloopstate = LoopState::Outer;
			outercounter -= 1;
		}
		else
		{
			changeState(LoopState::Inner);
			outercounter = 0;
		}
		
	}
	
	if(loopstate == LoopState::Inner)
	{
		if(innercounter > 0)
		{
			changeState(SortingState::Swap);
			setCompared(innercounter, 0);
		}
	}

	if(loopstate == LoopState::Innerst)
	{
		if(iLeftChild(innerstroot) <= innercounter)
		{
			switch (innerststep)
			{
			case 0:
				innerstchild = iLeftChild(innerstroot);
				innerstswap = innerstroot;
				innerststep++;
			case 1:
				if(values[innerstswap] < values[innerstchild])
				{
					innerstswap = innerstchild;
				}
				setCompared(innerstswap, innerstchild);
				innerststep++;
				break;
			case 2:
				if(innerstchild+1 <= outercounter)
				{
					if(values[innerstswap] < values[innerstchild+1])
					{
						innerstswap = innerstchild+1;
					}
					setCompared(innerstswap, innerstchild+1);
					innerststep++;
					break;
				}
				innerststep++;
			case 3:
				if(innerstswap == innerstroot)
				{
					changeState(previousloopstate);
				}
				else
				{
					changeState(SortingState::Swap);
				}
			}
		}
		else
		{
			changeState(previousloopstate);
		}
	}
}

inline int HeapSort::iParent(int i) 
{
	return (i - 1) / 2;
}

inline int HeapSort::iLeftChild(int i) 
{
	return (2 * i) + 1;
}

inline int HeapSort::iRightChild(int i) 
{
	return (2 * i) + 2;
}