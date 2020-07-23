#pragma once
#include "heap.hpp"
#include <algorithm>

using namespace std;

HeapSort::HeapSort(const int width, const int height)
	: SortWindow(width, height)
{
	innercounter = values.size()-1;
	outercounter = iParent(innercounter) + 1;
}

void HeapSort::Swap()
{
	switch (loopstate)
	{
	case LoopState::Inner :
		swap(values[innercounter], values[0]);
		setCompared(innerstroot, innerstswap);
		addSorted(innercounter);
		innercounter--;
		changeState(SortingState::Compare);
		changeState(LoopState::Innerst);
		innerstroot = 0;
		previousloopstate = LoopState::Inner;
		break;
	case LoopState::Innerst :
		swap(values[innerstroot], values[innerstswap]);
		innerstroot = innerstswap;
		innerststep = 0;
		changeState(SortingState::Compare);
		break;
	}
}

void HeapSort::Compare()
{
	if(loopstate == LoopState::Outer)
	{
		outercounter -= 1;
		if(outercounter >= 0)
		{
			changeState(LoopState::Innerst);
			innerststep = 0;
			innerstroot = outercounter;
			previousloopstate = LoopState::Outer;
		}
		else
		{
			bool ih = is_heap(begin(values), end(values));//TODO: rm dbg & inc
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
					//TODO: Compare(); for the below also
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