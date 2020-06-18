#include "quick.hpp"

using namespace std;

pair<int, int> QuickSort::findValidPartition()
{
	pair<int, int> partition{-1,-1};
	const vector<bool>& sortedindices = getSortedIndices();
	
	for(int index=0; index < sortedindices.size(); index++)
	{
		if(partition.first == -1)
		{
			if(!sortedindices[index])
				partition.first = index;
		}
		else
		{
			if(sortedindices[index])
			{
				partition.second = index;
				return partition;
			}
		}
	}

	if(partition.first > partition.second)
		partition.second = sortedindices.size();
	else
		partition.second = 0;

	return partition;
}

QuickSort::QuickSort(const int width, const int height)
	: SortWindow(width, height)
{
}

void QuickSort::Swap()
{
	if(loopstate == LoopState::Inner)
	{
		swap(values[innercounter-1], values[storeindex]);
		setCompared(innercounter-1, storeindex);
		storeindex++;
	}
	else
	{
		swap(values[pivotindex], values[--storeindex]);
		setCompared(pivotindex, storeindex);
		addSorted(storeindex);
	}
	changeState(SortingState::Compare);
}

void QuickSort::Compare()
{
	if(loopstate == LoopState::Outer)
	{
		pair<int, int> partition = findValidPartition();
		if(partition.first == partition.second-1)
		{
			if(partition.first == -1)
			{
				changeState(SortingState::Done);
				return;
			}
			addSorted(partition.first);
			setCompared(partition.first, partition.first);
			return;
		}
		pivotindex = partition.first;
		storeindex = pivotindex + 1;
		innercounter = storeindex;
		pastendindex = partition.second;
		changeState(LoopState::Inner);
	}

	if(innercounter < pastendindex)
	{
		if(values[innercounter] < values[pivotindex])
			changeState(SortingState::Swap);
		innercounter++;
		setCompared(innercounter, pivotindex);
	}
	else
	{
		changeState(SortingState::Swap);
		changeState(LoopState::Outer);
	}
}
